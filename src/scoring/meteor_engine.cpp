/**
 * @file meteor_engine.cpp
 * @brief High-performance C++ implementation of the METEOR evaluation metric.
 * 
 * Features:
 * - Multi-stage alignment (Exact, Stem, Synonym).
 * - Thread-local Snowball stemmers for OpenMP safety.
 * - Efficient in-memory synonym hashmap using IndoWordNet data.
 */

#include "scoring/meteor.h"
#include "preprocessing/stemmer.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <set>
#include <cmath>
#include <algorithm>
#include <mutex>

#ifdef _OPENMP
#include <omp.h>
#endif

/**
 * @class SynonymManager
 * @brief Handles loading and querying of language-specific synsets.
 */
class SynonymManager {
private:
    // Structure: language -> {word -> list of synset_ids}
    std::unordered_map<std::string, std::unordered_map<std::string, std::vector<int>>> synsets;

public:
    /**
     * @brief Loads synsets from a space-delimited file.
     * @param lang Language code ("hi", "ta").
     * @param filepath Path to the synset file.
     */
    void LoadSynsets(const std::string& lang, const std::string& filepath) {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            std::cerr << "[METEOR Error] Could not open synset file: " << filepath << std::endl;
            return;
        }

        std::string line;
        int synset_count = 0;
        while (std::getline(file, line)) {
            if (line.empty()) continue;
            std::stringstream ss(line);
            std::string word;
            while (ss >> word) {
                synsets[lang][word].push_back(synset_count);
            }
            synset_count++;
        }
    }

    /**
     * @brief Checks if two words belong to the same synset.
     * @param lang Language code.
     * @param w1 Word 1.
     * @param w2 Word 2.
     * @return true if they share a synset ID, false otherwise.
     */
    bool AreSynonyms(const std::string& lang, const std::string& w1, const std::string& w2) {
        if (w1 == w2) return true;
        
        auto it_lang = synsets.find(lang);
        if (it_lang == synsets.end()) return false;

        auto it1 = it_lang->second.find(w1);
        auto it2 = it_lang->second.find(w2);
        if (it1 == it_lang->second.end() || it2 == it_lang->second.end()) return false;

        // Intersection check: shared synset ID = synonyms
        const auto& ids1 = it1->second;
        const auto& ids2 = it2->second;
        for (int id1 : ids1) {
            for (int id2 : ids2) {
                if (id1 == id2) return true;
            }
        }
        return false;
    }
};

// Global synonym singleton
static SynonymManager g_synonyms;
static std::string g_resource_dir;

/**
 * @brief Thread-local cache for Snowball stemmers.
 * Ensures that each OpenMP thread has its own stemmer instance, as the
 * underlying Snowball C library environments are not thread-safe.
 */
static thread_local std::unordered_map<std::string, Stemmer*> tl_stemmers;

/**
 * @brief Retrieves or lazily creates a stemmer for the current thread.
 */
static Stemmer* GetStemmer(const std::string& lang_code) {
    if (tl_stemmers.count(lang_code)) return tl_stemmers[lang_code];

    StemmerLanguage sl;
    if (lang_code == "hi") sl = LANG_HINDI;
    else if (lang_code == "ta") sl = LANG_TAMIL;
    else return nullptr;

    Stemmer* s = StemmerCreate(sl);
    tl_stemmers[lang_code] = s;
    return s;
}

extern "C" {

/**
 * @brief Global initialization of METEOR scoring resources.
 */
int InitMeteor(const char* synonym_dir) {
    g_resource_dir = synonym_dir ? synonym_dir : "data/indowordnet";
    g_synonyms.LoadSynsets("hi", g_resource_dir + "/hi_synsets.txt");
    g_synonyms.LoadSynsets("ta", g_resource_dir + "/ta_synsets.txt");
    return 0;
}

/**
 * @struct Alignment
 * @brief Represents a single word-to-word mapping between candidate and reference.
 */
struct Alignment {
    int cand_idx; ///< Position in candidate tokens.
    int ref_idx;  ///< Position in reference tokens.
    int stage;    ///< Matching stage (1: Exact, 2: Stem, 3: Synonym).
};

/**
 * @brief Computes the METEOR score for a single sentence pair.
 * 
 * Algorithm:
 * 1. Multi-pass greedy alignment.
 * 2. Precision and Recall calculation (favoring recall).
 * 3. Fragmentation penalty based on matching 'chunks'.
 */
double ComputeMeteorScore(const char* candidate, const char* reference, const char* lang) {
    if (!candidate || !reference || !lang) return 0.0;

    std::string l(lang);
    
    // Internal helper for simple tokenization
    auto tokenize = [](const std::string& s) {
        std::vector<std::string> tokens;
        std::stringstream ss(s);
        std::string t;
        while (ss >> t) tokens.push_back(t);
        return tokens;
    };

    std::vector<std::string> cand_tokens = tokenize(candidate);
    std::vector<std::string> ref_tokens = tokenize(reference);

    if (cand_tokens.empty() || ref_tokens.empty()) return 0.0;

    const int n_cand = static_cast<int>(cand_tokens.size());
    const int n_ref = static_cast<int>(ref_tokens.size());

    std::vector<bool> cand_matched(n_cand, false);
    std::vector<bool> ref_matched(n_ref, false);
    std::vector<Alignment> alignments;

    // --- Phase 1: Exact Match ---
    for (int i = 0; i < n_cand; i++) {
        for (int j = 0; j < n_ref; j++) {
            if (!ref_matched[j] && cand_tokens[i] == ref_tokens[j]) {
                cand_matched[i] = true;
                ref_matched[j] = true;
                alignments.push_back({i, j, 1});
                break;
            }
        }
    }

    // --- Phase 2: Stem Match ---
    Stemmer* s = GetStemmer(l);
    if (s) {
        for (int i = 0; i < n_cand; i++) {
            if (cand_matched[i]) continue;
            std::string c_stem = StemmerStemWord(s, cand_tokens[i].c_str());
            for (int j = 0; j < n_ref; j++) {
                if (!ref_matched[j]) {
                    std::string r_stem = StemmerStemWord(s, ref_tokens[j].c_str());
                    if (c_stem == r_stem) {
                        cand_matched[i] = true;
                        ref_matched[j] = true;
                        alignments.push_back({i, j, 2});
                        break;
                    }
                }
            }
        }
    }

    // --- Phase 3: Synonym Match ---
    for (int i = 0; i < n_cand; i++) {
        if (cand_matched[i]) continue;
        for (int j = 0; j < n_ref; j++) {
            if (!ref_matched[j] && g_synonyms.AreSynonyms(l, cand_tokens[i], ref_tokens[j])) {
                cand_matched[i] = true;
                ref_matched[j] = true;
                alignments.push_back({i, j, 3});
                break;
            }
        }
    }

    const int matches = static_cast<int>(alignments.size());
    if (matches == 0) return 0.0;

    // Harmonic mean of P and R (9:1 weight for recall)
    double P = static_cast<double>(matches) / n_cand;
    double R = static_cast<double>(matches) / n_ref;
    double F_mean = (10.0 * P * R) / (R + 9.0 * P);

    // Sorting by candidate order to count contiguous chunks
    std::sort(alignments.begin(), alignments.end(), [](const Alignment& a, const Alignment& b) {
        return a.cand_idx < b.cand_idx;
    });

    int chunks = 1;
    for (size_t i = 1; i < alignments.size(); i++) {
        // A disjoint reference index indicates a fragmentation breakpoint.
        if (alignments[i].ref_idx != alignments[i-1].ref_idx + 1) {
            chunks++;
        }
    }

    // Standard METEOR fragmentation penalty
    double frag = static_cast<double>(chunks) / matches;
    double penalty = 0.5 * std::pow(frag, 3);

    return F_mean * (1.0 - penalty);
}

/**
 * @brief Teardown for thread-local stemmers.
 */
void CleanupMeteor() {
    for (auto& pair : tl_stemmers) {
        if (pair.second) StemmerDelete(pair.second);
    }
    tl_stemmers.clear();
}

} // extern "C"
