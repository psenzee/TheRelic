#include "Models.h"
#include "OverheadCamera.h"
#include "GraphicsDevice.h"
#include "LocalGameServices.h"
#include "DeviceModel.h"
#include "DeviceTexture.h"
#include "DrawList.h"

static Matrix MATRIX_IDENTITY;

IAnimator *StaticModel::GlobalAnimator                 = 0;
bool       StaticModel::GlobalAnimatorMatrixIsIdentity = true;

const Matrix &StaticModel::GetAnimatorMatrix()
{
    return !GlobalAnimator ? MATRIX_IDENTITY : GlobalAnimator->GetMatrix();
}

void StaticModel::GlobalUpdate(const GameTime &time)
{
    if (!GlobalAnimator)
        GlobalAnimatorMatrixIsIdentity = true;
    else
        GlobalAnimatorMatrixIsIdentity = (GlobalAnimator->Update(time) == MATRIX_IDENTITY);
}

Transform StaticModel::GetTransform() const
{
    if (!GlobalAnimatorMatrixIsIdentity)
        return transform;
    return Transform(transform.matrix * GetAnimatorMatrix());
}

StaticModel::StaticModel(DeviceModel *model,
            std::vector<Texture *> &textures, const Transform &transform,
            Collider *collider, RenderStates *states)
   : model(model), textures(textures), transform(transform), collider(collider), states(states), transformidentity(false), cullinvisible(true)
{
    transformidentity = (transform.matrix == MATRIX_IDENTITY);
    bounds = model->GetBounds();
    transparentz = (bounds.minimum.z + bounds.maximum.z) * 0.5f;
}

bool StaticModel::SetProperty(const char *key, const char *value)
{
    if      (!key || !value) return false;
    else if (strcmp(key, "cull-invisible") == 0)
    {
        if      (strcmp(value, "true")  == 0) { cullinvisible = true;  return true; }
        else if (strcmp(value, "false") == 0) { cullinvisible = false; return true; }
        return false;
    }
    return model->SetProperty(key, value);
}

void StaticModel::Render(GraphicsDevice &device, const Matrix &transform, const OverheadCamera &camera, bool now)
{
    /*static*/ Matrix m;
//     Matrix matrix(GetAnimatorMatrix() * transform);
    const Matrix *matrix = &transform;
    if (!GlobalAnimatorMatrixIsIdentity)
    {
        m = GetAnimatorMatrix() * transform;
        matrix = &m;
    }
    if (!cullinvisible || model->IsVisible(*matrix, camera))
    {
        Texture *texture = 0;
        if (!textures.empty() && textures[0])
            texture = textures[0];
        if (!now)
            DrawList::GetDrawList()->Add(*matrix, model, texture, *states, transparentz);            
        else
        {
            states->Apply(device);
            if (texture)
                texture->Set(device);
            model->Render(device, *matrix, camera);
            states->Unapply(device);
        }
    }
}

bool StaticModel::IsVisible(const Matrix &transform, const OverheadCamera &camera) const
{
    return model->IsVisible(GetAnimatorMatrix() * transform, camera);
}

void StaticModel::Update(const GameTime &time)
{
}

void ModelAnimator::Render(GraphicsDevice &device, const Matrix &transform, const OverheadCamera &camera, bool now)
{
    model->Render(device, GetTransform().matrix * transform, camera, now);
}

bool ModelAnimator::IsVisible(const Matrix &transform, const OverheadCamera &camera) const
{
    return model->IsVisible(GetTransform().matrix * transform, camera);    
}

void ModelAnimator::Update(const GameTime &time)
{
    transform = Transform(animator->Update(time));
    model->Update(time);
}

ModelAnimator::ModelAnimator(IModel *model, IAnimator *animator) 
    : model(model->CopyIfNecessary()), animator(animator->CopyIfNecessary())
{
}

ModelAnimator::ModelAnimator(ModelAnimator *model) 
    : model(model->GetModel()->CopyIfNecessary()), 
        transform(model->GetTransform()), animator(model->animator->CopyIfNecessary())
{
}


RenderableModel::RenderableModel(ContentCache *content, const char *name, const char *file,
                       std::vector<Texture *> textures, const Transform &transform,
                       IAnimator *animator, Collider *collider, RenderStates *states)
{
    model = new StaticModel(content->GetModel(file), textures, transform, collider, states);
    if (animator)
        model = new ModelAnimator(model, animator);
}

RenderableModel::RenderableModel(RenderableModel *model) : model(model->GetModel()->CopyIfNecessary())
{
}

void RenderableModel::Update(const GameTime &time)
{
    model->Update(time);
}

void RenderableModel::Render(GraphicsDevice &device, const Matrix &transform, const OverheadCamera &camera, bool now)
{
    model->Render(device, transform, camera, now);
}

bool RenderableModel::IsVisible(const Matrix &transform, const OverheadCamera &camera) const
{
    return model->IsVisible(transform, camera);
}

IModel *RenderableModel::CopyIfNecessary()
{
    if (!GetAnimator()) return this;
    return new RenderableModel(this);
}
