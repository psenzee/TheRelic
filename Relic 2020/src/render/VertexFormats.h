#pragma once

/* VertexFormats
 
 format
 
 non case sensitve
 current:
 starts with
 
 I      - structure: Interleaved - currently the only option, required
 
 then
 
 L or S - triangle set types: List (3 vertices per) or Strips (2 initial then 1 per subsequent)
 
 the following can occur in any order:
 
 Pn     - Position - where n is 1-4.  Only 1
 Nn     - Normal - where n is 1-4.  Only 1?
 Tn     - Texture - coor where n is 1-4.  Repeatable up to the max textures
 Cn     - Color - where n is 1-4.  Repeatable number up to the max textures
 
 */

#include <string_view>
#include <vector>
#include <algorithm>

class VertexFormat;

struct VertexFormats
{
    const VertexFormat *format(std::string_view s);

private:
    typedef std::pair<std::string_view, const VertexFormat *> entry_t;
    typedef std::vector<entry_t>                              registry_t;
    typedef std::vector<entry_t>::const_iterator              registry_iter_t;

    registry_t      _registry;
    
    registry_iter_t _find(std::string_view s);
    void            _add(const VertexFormat *vf);
};

int test_vertex_formats();
