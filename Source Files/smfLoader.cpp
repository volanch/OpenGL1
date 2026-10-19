#include "smfLoader.h"
#include <fstream>
#include <sstream>

bool LoadSMF(const std::string& path, SmfModel& model, std::string& error) {
    std::ifstream in(path);
    if (!in) {
        error = "cannot open file '" + path + "'";
        return false;
    }

    model.vertices.clear();
    model.triangles.clear();

    std::string line;
    int lineNo = 0;
    while (std::getline(in, line)) {
        ++lineNo;
        const auto hash = line.find('#');
        if (hash != std::string::npos) line.erase(hash);

        std::istringstream ss(line);
        std::string tag;
        if (!(ss >> tag)) continue;

        if (tag == "v") {
            glm::vec3 p;
            if (!(ss >> p.x >> p.y >> p.z)) {
                error = "line " + std::to_string(lineNo) + ": bad vertex";
                return false;
            }
            model.vertices.push_back(p);
        } else if (tag == "f") {
            std::vector<unsigned> idx;
            std::string tok;
            while (ss >> tok) {
                int i = 0;
                try { i = std::stoi(tok); }
                catch (...) {
                    error = "line " + std::to_string(lineNo) + ": bad face index '" + tok + "'";
                    return false;
                }
                const int n = static_cast<int>(model.vertices.size());
                i = (i < 0) ? n + i : i - 1;
                if (i < 0 || i >= n) {
                    error = "line " + std::to_string(lineNo) + ": face index out of range";
                    return false;
                }
                idx.push_back(static_cast<unsigned>(i));
            }
            if (idx.size() < 3) {
                error = "line " + std::to_string(lineNo) + ": face needs at least 3 vertices";
                return false;
            }
            for (size_t k = 1; k + 1 < idx.size(); ++k)
                model.triangles.emplace_back(idx[0], idx[k], idx[k + 1]);
        }
    }

    if (model.vertices.empty() || model.triangles.empty()) {
        error = "file contains no triangles";
        return false;
    }
    return true;
}
