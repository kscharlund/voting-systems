#include <iostream>
#include <vector>
#include <map>
#include <algorithm>
#include <cassert>

// Input specification:
// First one line with N, the number of candidates.
// Then N lines with the names of each candidate.
// Then one line with M, the number of voters.
// Then for each voter, up to N + 1 lines.
// Each line consists of a positive number indicating a rank followed by a
// candidate name, or 0 to indicate end of preferences.

#define DEBUG 0

int main(int argc, char *argv[])
{
    // Read number of candidates.
    int nCandidates = 0;
    std::cin >> nCandidates;
    std::string tmp;
    std::getline(std::cin, tmp);

    // Read candidate names.
    std::map<std::string, int> candidates;
    std::map<int, std::string> candidateNames;
    for (int ii = 0; ii < nCandidates; ++ii)
    {
        std::string name;
        std::getline(std::cin, name);
        candidates.insert(std::make_pair(name, ii));
        candidateNames.insert(std::make_pair(ii, name));
    }

#if DEBUG
    std::cerr << "Candidates" << std::endl;
    for (auto const &p : candidates)
    {
        std::cerr << p.first << " " << p.second << std::endl;
    }
    std::cerr << std::endl;
#endif

    // Create pairwise winner matrix.
    std::vector<std::vector<int> > beats(nCandidates);
    for (auto &v : beats)
    {
        v.resize(nCandidates);
    }

    // Read and count votes.
    int nVoters = 0;
    std::cin >> nVoters;
    for (int ii = 0; ii < nVoters; ++ii)
    {
        std::vector<std::pair<int, int > > preferences;
        while (true)
        {
            int rank;
            std::cin >> rank;
            if (rank <= 0)
            {
                break;
            }

            std::string name;
            std::getline(std::cin, name);
            name.erase(
                name.begin(),
                std::find_if(
                    name.begin(),
                    name.end(),
                    std::not1(std::ptr_fun<int, int>(std::isspace))));
            auto cand = candidates.find(name);
            assert(cand != candidates.end());
            int id = cand->second;

            preferences.push_back(std::make_pair(rank, id));
        }
        std::sort(preferences.begin(), preferences.end());

        std::vector<int> ranked(nCandidates, 0);
        for (auto const &pref : preferences)
        {
            ranked[pref.second] = 1;
            for (int jj = 0; jj < nCandidates; ++jj)
            {
                if (!ranked[jj])
                {
                    beats[pref.second][jj] += 1;
                }
            }
        }
    }

#if DEBUG
    std::cerr << "Pairwise preferences" << std::endl;
    for (int ii = 0; ii < nCandidates; ++ii)
    {
        for (int jj = 0; jj < nCandidates; ++jj)
        {
            std::cerr << beats[ii][jj] << " ";
        }
        std::cerr << std::endl;
    }
    std::cerr << std::endl;
#endif

    // Create path strength matrix.
    std::vector<std::vector<int> > paths(nCandidates);
    for (auto &v : paths)
    {
        v.resize(nCandidates);
    }

    // Calculate path strengths.
    for (int ii = 0; ii < nCandidates; ++ii)
    {
        for (int jj = 0; jj < nCandidates; ++jj)
        {
            if (ii != jj)
            {
                if (beats[ii][jj] > beats[jj][ii])
                {
                    paths[ii][jj] = beats[ii][jj];
                }
            }
        }
    }
    for (int ii = 0; ii < nCandidates; ++ii)
    {
        for (int jj = 0; jj < nCandidates; ++jj)
        {
            if (ii != jj)
            {
                for (int kk = 0; kk < nCandidates; ++kk)
                {
                    if (ii != kk && jj != kk)
                    {
                        paths[jj][kk] = std::max(paths[jj][kk], std::min(paths[jj][ii], paths[ii][kk]));
                    }
                }
            }
        }
    }

#if DEBUG
    std::cerr << "Path strenghts" << std::endl;
    for (int ii = 0; ii < nCandidates; ++ii)
    {
        for (int jj = 0; jj < nCandidates; ++jj)
        {
            std::cerr << paths[ii][jj] << " ";
        }
        std::cerr << std::endl;
    }
    std::cerr << std::endl;
#endif

    // Determine ranking.
    std::vector<std::pair<int, int> > ranking;
    for (int ii = 0; ii < nCandidates; ++ii)
    {
        int nWins = 0;
        for (int jj = 0; jj < nCandidates; ++jj)
        {
            if (ii != jj)
            {
                if (paths[ii][jj] > paths[jj][ii])
                {
                    ++nWins;
                }
            }
        }
        ranking.push_back(std::make_pair(-nWins, ii));
    }
    std::sort(ranking.begin(), ranking.end());
    for (auto const &p : ranking)
    {
        std::cout << candidateNames[p.second] << " (" << (-p.first) << " wins)" << std::endl;
    }

    return 0;
}
