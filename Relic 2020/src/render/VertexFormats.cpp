#include "VertexFormats.h"
#include "VertexFormat.h"
#include "VertexFormatGL.h"

#include <string>
#include <string_view>
#include <algorithm>
#include <span>

const VertexFormat *VertexFormats::format(std::string_view s)
{
    auto i = _find(s);
    if (i == _registry.end()) {
        VertexFormat *vf = new VertexFormat;
        if (vf->parse(s)) {
            _add(vf);
            return vf;
        } else {
            // handle error
            return nullptr;
        }
    }
    return (*i).second;
}

VertexFormats::registry_iter_t VertexFormats::_find(std::string_view s)
{
    return std::upper_bound(
        _registry.begin(), _registry.end(), entry_t(s, nullptr),
        [](const entry_t &a, const entry_t &b) {
            return strcasecmp(a.first.data(), b.first.data()) < 0;
        });
}

void VertexFormats::_add(const VertexFormat *vf)
{
    if (vf) {
        _registry.push_back(entry_t(vf->format, vf));
        std::sort(_registry.begin(), _registry.end(), [](const entry_t &a, const entry_t &b) {
            return strcasecmp(a.first.data(), b.first.data()) < 0;
        });
    }
}

int test_vertex_formats()
{
    VertexFormats vfs;
    const char *tests[] = { "IL P3 N3:f16 T2 C3 C3 T2", "ILP3N3T2C3C3T2", "seveh", "ISP4T2 C3", "ISP4T2 C3", "MSP4T2C3", 0 };
    for (const char **t = tests; *t; t++) {
        const VertexFormat *vf = vfs.format(*t);
        if (!vf) {
            std::cout << "failed" << std::endl;
        } else {
            std::cout << *vf << std::endl;
            VertexFormatGL vfgl(*vf);
            vfgl.codeLayOutArray(std::cout);
            vfgl.codeBindProgram(std::cout, "mProgramId");
        }
        std::cout << "--------" << std::endl << std::endl;
    }
    return 0;
}
