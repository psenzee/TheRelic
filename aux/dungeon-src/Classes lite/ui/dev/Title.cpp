/*
 *  Title.cpp
 *  GLGravity
 *
 *  Created by Paul Senzee on 1/10/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include "Title.h"

/*
void GameState::Narration()
{
    const float START = -200.0f, END = 700.0f;
    static float y     = START;
    static int   state = 0;
    /*
     #define NARRATION_COLOR "\\#ffffff"    
     
     const char *pages[] = 
     {
     NARRATION_COLOR "In the pinnacle of the\n"
     "Library of Babel, sage and\n"
     "mystic 8 balls come to be.",
     
     NARRATION_COLOR "They emerge wisened and aged\n"
     "from their long descent\n"
     "through the infinite studies\n"
     "and hanging gardens of the\n"
     "library down to earth.",
     
     NARRATION_COLOR "Eight balls are black.\n"
     "But we live in a world of\n"
     "exception and so does\n"
     "Indigo Ocho.",
     
     NARRATION_COLOR "It's said that the road of\n"
     "excess leads to the palace of\n"
     "wisdom.\n\n"
     "This is the road of excess.",
     
     NARRATION_COLOR "How far will you go?\n",
     
     0
     };
     
     #undef NARRATION_COLOR    
     * /
    
#define NARRATE_COLOR  "\\#ffffff"
#define NARRATE_BLACK  "\\#000000"
#define NARRATE_INDIGO "\\#400080"
#define NARRATE_YELLOW "\\#ffff00"    
    
    const char *pages[] = 
    {
        NARRATE_COLOR "In the pinnacle of the\n"
        NARRATE_BLACK "Library of Babel" NARRATE_COLOR ", sage and\n"
        "mystic 8 balls come to be.\n\n"
        
        NARRATE_COLOR "They emerge wisened and aged\n"
        "from their long descent\n"
        "through the infinite studies\n"
        "and hanging gardens of the\n"
        "library down to earth.\n\n"
        
        "Eight balls are " NARRATE_BLACK "black.\n" NARRATE_COLOR
        "But we live in a world of\n"
        "exception and so does\n"
        NARRATE_INDIGO "Indigo Ocho.\n\n"
        
        NARRATE_COLOR "It's said that the road of\n"
        "excess leads to the palace of\n"
        "wisdom.\n\n"
        
        "This is the road of excess.\n\n\n"
        
        NARRATE_YELLOW "How far will you go?\n",
        
        0
    };
    
#undef NARRATE_COLOR    
#undef NARRATE_BLACK
#undef NARRATE_INDIGO
#undef NARRATE_YELLOW    
    
    if (pages[state])
    {    
        if (y < END)
            ui->DrawString(pages[state], Vector2(180.0f, y), 0.5f);
        else
        {
            y = START;
            state++;
        }
        y += 2.0f;
        if (y == 600.0f)
        {
            // title fade in?
        }
    }
    else
    {        
        // move on to the game
        this->state = GAME;
        effects.FadeBackgroundToGame();
    }
}
*/
