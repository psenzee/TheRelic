#include "Renderable.h"
#include "Models.h"
#include "GraphicsDevice.h"
#include "DeviceModel.h"

static AABox GetBounds(const std::vector<IModel *> &list)
{
    AABox box;
    for (std::vector<IModel *>::const_iterator i = list.begin(), e = list.end(); i != e; ++i)
    {
        if (*i)
        {
            DeviceModel *model = (*i)->GetDeviceModel();
            if (model) box.insert(model->GetBounds());        
        }
    }
    return box;    
}

Renderable::Renderable(const char *name, std::vector<IModel *> models, const Transform &transform, std::vector<Event *> events)
    : name(name), models(models), transform(transform), events(events)
{
    bounds = ::GetBounds(models);
}

void Renderable::Update(const GameTime &time)
{
    if (!models.empty()) 
        for (std::vector<IModel *>::iterator i = models.begin(), e = models.end(); i != e; ++i)
            if ((*i) != 0) (*i)->Update(time);
}

bool Renderable::IsStatic() const
{
    for (std::vector<IModel *>::const_iterator i = models.begin(), e = models.end(); i != e; ++i)
        if ((*i) != 0 && !(*i)->IsStatic())
            return false;
    return true;
}

IRenderable *Renderable::CopyIfNecessary()
{
    std::vector<IModel *> newmodels;    
    if (!models.empty())
    {
        for (std::vector<IModel *>::iterator i = models.begin(), e = models.end(); i != e; ++i)
            if ((*i) != 0) newmodels.push_back((*i)->CopyIfNecessary());
    }
    return new Renderable(name.c_str(), newmodels, transform, events);
}

void Renderable::Render(GraphicsDevice &device, const Matrix &transform, const OverheadCamera &camera, bool now)
{
    if (!models.empty()) 
        for (std::vector<IModel *>::iterator i = models.begin(), e = models.end(); i != e; ++i)
            if ((*i) != 0) (*i)->Render(device, /*this->transform.matrix * */transform, camera, now);
}

bool Renderable::IsVisible(const Matrix &transform, const OverheadCamera &camera) const
{
    if (!models.empty())
        for (std::vector<IModel *>::const_iterator i = models.begin(), e = models.end(); i != e; ++i)
            if ((*i) != 0 && (*i)->IsVisible(/*this->transform.matrix * */transform, camera))
                return true;
    return false;
}

AABox Renderable::GetBounds() const
{
    return bounds;
}

bool Renderable::SetProperty(const char *key, const char *value)
{
    if (!key) return false;
    properties[key] = value;
    return true;
}

const char *Renderable::GetProperty(const char *key) const
{
    if (!key) return 0;
    std::map<std::string, std::string>::const_iterator i = properties.find(key), e = properties.end();
    return i == e ? 0 : (*i).second.c_str();
}