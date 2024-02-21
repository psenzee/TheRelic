#include "UiBasicControl.h"
#include "UiCore.h"
#include "events/EventDispatcher.h"

bool UiBasicControl::Notify(UiCore &core, const UiEvent &event)
{
    if (!UiEvent::IsFundamentalEvent(event.type) || (IsVisible() && IsActive() && ((GetState().IsFocused() && 
          GetFocusedBounds().Contains(event.at)) || GetBounds().Contains(event.at))))
    {
        GetState().Notify(event);
        UiEvent ne(event);
        ne.info = GetName();
        // we set off a real event here 
        core.GetEventDispatcher().Fire(GetName(), UiEvent::GetEventTypeName(ne.type));
        switch (GetState().GetState())
        {
        case UiState::FOCUSED:  core.GetEventDispatcher().Fire(GetName(), "FOCUSED");  break;
        case UiState::SELECTED: core.GetEventDispatcher().Fire(GetName(), "SELECTED"); break;
        }
        return true;
    }
    return false;
}