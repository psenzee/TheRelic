#include "UiCollection.h"
#include "UiCore.h"
#include "events/EventDispatcher.h"
#include <algorithm>

UiCollection::~UiCollection()
{
    std::vector<IUiControl *>::iterator i = mItems.begin(), e = mItems.end();
    for (; i != e; ++i)
        delete *i;
    mItems.clear();
}

void UiCollection::NotifyVisible(bool visible)
{
    std::vector<IUiControl *>::iterator i = mItems.begin(), e = mItems.end();
    for (; i != e; ++i)
        (*i)->NotifyVisible(visible);
}

void UiCollection::DestroyChildren()
{
    std::vector<IUiControl *>::iterator i = mItems.begin(), e = mItems.end();
    for (; i != e; ++i)
    {
        (*i)->DestroyChildren();
        delete *i;
    }
    mItems.clear();
    mLastNotified = 0;
}

bool UiCollection::Notify(UiCore &core, const UiEvent &event)
{
    if (!IsVisible() || !IsActive())
        return false;
    UiEvent ne(event);
    ne.at = event.at - GetBounds().minimum;
    UiBounds bounds;
    std::vector<IUiControl *>::iterator i = mItems.begin(), e = mItems.end();
    IUiControl *notified = 0;
    if (mLastNotified && mLastNotified->Notify(core, ne)) // test the currently active one first
       notified = mLastNotified;
    else
    {
        for (; i != e; ++i)
        {
            if (!(*i)) continue;
            if ((*i)->Notify(core, ne))
            {
                notified = *i;
                break;
            }
        }
    }
    if (mLastNotified && notified != mLastNotified && 
        UiEvent::IsFundamentalEvent(event.type))
    {
        UiEvent left(ne);
        left.type = UiEvent::CLICK_LEAVE;
        mLastNotified->Notify(core, left);
        if (notified && event.type == UiEvent::CLICK_MOVED)
        {
            UiEvent left(ne);
            left.type = UiEvent::CLICK_ENTERED;
            notified->Notify(core, left);
        }
    }
    if (UiEvent::IsFundamentalEvent(event.type))
        mLastNotified = notified;
    if (!notified)
        return false;
    if (GetBounds().Contains(event.at))
    {
        UiEvent ne(event);
        ne.info = GetName();
        // we set off a real event here
        //printf("'%s'\n", ne.info.c_str());
        core.GetEventDispatcher().Fire(GetName(), UiEvent::GetEventTypeName(ne.type));
        return true;
    }
    return false;
}

int UiCollection::RenderRenderable(UiCore &core)
{
    int count = 0;
    if (IsVisible())
    {
        std::vector<IUiControl *>::iterator i = mItems.begin(), e = mItems.end();
        for (; i != e; ++i)
        {
            if (!(*i)) continue;
            count += (*i)->RenderRenderable(core);
        }
    }
    return count;
}

int UiCollection::Render(UiCore &core)
{
    int count = 0;
    if (IsVisible())
    {
        IUiControl *focused = 0;
        std::vector<IUiControl *>::iterator i = mItems.begin(), e = mItems.end();
        for (; i != e; ++i)
            if ((*i)->GetState().IsFocused())
                focused = *i;
        i = mItems.begin();
        for (; i != e; ++i)
        {
            if (!(*i) || (*i == focused))
                continue;
            count += (*i)->Render(core);
        }
        if (focused)
            count += focused->Render(core);
    }
    return count;
}

void UiCollection::Update(/*..*/)
{
    if (IsActive())
    {
        std::vector<IUiControl *>::iterator i = mItems.begin(), e = mItems.end();
        for (; i != e; ++i)
        {
            if (!(*i)) continue;
            (*i)->Update(/*..*/);
        }
    }
}

void UiCollection::AddChild(IUiControl *control)
{ 
    mItems.push_back(control);
    control->SetParent(this);
}

bool UiCollection::RemoveChild(IUiControl *control)
{ 
    size_t sz = mItems.size();
    mItems.erase(std::remove(mItems.begin(), mItems.end(), control), mItems.end());
    control->SetParent(0);
    return sz != mItems.size();
}
