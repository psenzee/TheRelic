/*
 *  Menu.cpp
 *  GLGravity
 *
 *  Created by Paul Senzee on 1/9/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include "Menu.h"
#include "GameUI.h"
#include "OverheadCamera.h"
    
Menu::Menu(const char *title, int rowheight) : title(title ? title : ""), rowheight(rowheight), visible(false), fader(0.0f)
{
}
        
void Menu::Add(const MenuItem &item)
{
    items.push_back(item);
}

void Menu::SetVisible(bool v)
{ 
    if (visible == v)
        return;
    visible = v;
    fader.Fade(visible ? 1.0f : 0.0f, 1000);
}

void MenuItem::Draw(GameUI &ui, float alpha)
{
    float alpha = fader.GetValue();
    if (alpha < 0.01f)
        return;
    float start = 0.0f, size = 26.0f;
    int   count = 0;
    // we have a line about which it should be centered
    float centerline = -45.0f;
    start = centerline + items.size() * rowheight * 0.5f;
    for (std::vector<MenuItem>::iterator i = items.begin(), e = items.end(); i != e; ++i)
    {
        MenuItem &item = *i;
        if (!item.text.empty())
        {
            Vector2 at(0.0f, start - count * rowheight);
            AABox &box = item.bounds;
            const char *s = item.text.c_str();
            if (!box.valid()) // uninitialized bounds
            {
                box = ui.GetCenteredHitTestBox(s, at, size);
                box.expand(Vector3(size * 2.0f, math::max(rowheight - size, 0.0f), 0.0f));
            }
            ui.DrawStringCentered(s, at, alpha, size);
        }
        count++;
    }
}

bool MenuItem::HandleTouch(const Vector3 &pick)
{
    if (!active)
        return false;
    if (function && 
        pick.x >= bounds.minimum.x && pick.x <= bounds.maximum.x &&
        pick.y >= bounds.minimum.y && pick.y <= bounds.maximum.y)
    {
        // $TODO animation
        function(item);
        return true;
    }
    return false;
}
