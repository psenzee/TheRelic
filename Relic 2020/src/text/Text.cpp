#include "Text.h"
#include "Glyph.h"

#include "core/random.h"
#include "core/simplexml.h"
#include "core/global.h"

#include "render/OverheadCamera.h"
#include "render/DeviceTexture.h"
#include "render/Material.h"

Text::Text(GlyphWriter *writer, const char *s, const Vector2 &pos, const Vector2 &scale, const Vector3 &defaultColor, IAnimator *animator)
    : writer(writer), animator(animator)
{    
    // \#443355
    Vector4       color     = v4(defaultColor, 1.0f),
                  lastColor = color;
    Vector3       p         = Vector3(pos.x + 0.5f * scale.x, pos.y, 0.0f);
    size_t        sz        = strlen(s);
    char         *current   = new char [sz + 1],
                 *pcur      = current;
    
    memset(current, 0, sz + 1);    
    while (*s)
    {
        if (!GlyphWriter::ParseColor(&s, color))
            *pcur++ = *s++;
        else
        {
            if (*current)
            {
                Add(current, p, scale, lastColor);
                p = lists.back()->GetCursor();
            }
            pcur = current;
            memset(current, 0, sz + 1);
            lastColor = color;
        }
    }
    Add(current, p, scale, lastColor);
    delete [] current;
}

Text::~Text()
{
    for (std::vector<GlyphDrawList *>::const_iterator i = lists.begin(), e = lists.end(); i != e; ++i)
        delete *i;
    lists.clear();
}

void Text::MakePartyText(const char *s, char *buffer, int seed)
{
    if (!s || !*s)
        return;    
    const char *colors[] = { "\\#ff0000", "\\#00ff00", "\\#0000ff", "\\#ffff00" };
    size_t count = sizeof(colors) / sizeof(colors[0]);
    static core::Random r(0);
    if (seed != 0)
        r = core::Random(seed);
    char *pb = buffer;
    int prev = -1;
    for (; *s; s++)
    {
        int index = static_cast<int>(r.integer() % count);
        if (index == prev)
            index = (index + 1) % (int)count; // make sure we don't have the same color twice adjacent
        pb += snprintf(pb, 16, "%s%c", colors[index], *s);
        prev = index;
    }
}

void Text::StripColor(const char *s, char *buffer)
{
    char    *p = buffer;
    Vector4  color;
    while (*s)
        if (!GlyphWriter::ParseColor(&s, color))
            *p++ = *s++;
    *p = 0;
}

void Text::Update(const GameTime &time)
{
    if (animator)
        animator->Update(time);
}
    
void Text::ForceUpdate(const GameTime &time)
{
    if (animator)
        animator->ForceUpdate(time);
}

Vector3 Text::GetMin() const
{
    Vector3 min(FLT_MAX, FLT_MAX, FLT_MAX);
    for (std::vector<GlyphDrawList *>::const_iterator i = lists.begin(), e = lists.end(); i != e; ++i)
        min = (*i)->GetMin().minimum(min);
    return min;
}
    
Vector3 Text::GetMax() const
{
    Vector3 max(FLT_MIN, FLT_MIN, FLT_MIN);
    for (std::vector<GlyphDrawList *>::const_iterator i = lists.begin(), e = lists.end(); i != e; ++i)
        max = (*i)->GetMax().maximum(max);
    return max;
}
    
Vector3 Text::GetSize() const
{
    return GetMax() - GetMin();
}
    
void Text::Render(RenderContext &context, const Matrix &transform, const Vector4 &color)
{
    Matrix animated; animated.identity();
    Vector4 clr(color);
    if (animator)
    {
        animated  = animator->GetMatrix();
        clr      *= animator->GetColor();
    }
    Matrix nt(animated * transform);
    anchor = (nt * Vector3(1.0f, 1.0f, 1.0f));
    for (std::vector<GlyphDrawList *>::iterator i = lists.begin(), e = lists.end(); i != e; ++i)
        (*i)->Render(context, nt, clr);
}

void Text::Reset()
{
    if (animator)
        animator->Reset();
}

const Vector3 &Text::GetRenderAnchorPosition() const
{
    return anchor;
}

void Text::Add(const char *s, const Vector3 &pos, const Vector2 &scale, const Vector4 &color)
{
    if (s && *s)
    {
        Vector3 initial(pos);
        if (!lists.empty())
            initial = lists[0]->GetInitialPosition();
        initial.y = pos.y;
        lists.push_back(writer->Write(s, v2(initial), v2(pos), scale, color));
    }
}
