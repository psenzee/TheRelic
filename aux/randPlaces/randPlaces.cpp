#include <stdlib.h>
#include <stdio.h>
#include <conio.h>

#include "PlaceNameGenerator.h"

void main()
{
    PlaceNameGenerator places;
    core::Random random(35);

    FILE *file = fopen("placenames.txt", "w");
    for (int i = 0; i < 10000; i++)
        fprintf(file, "%s\n", places.GetPlaceName(random));
    fclose(file);
    getch();
}