#include "common.h"

void initDemo(void)
{
	spaceCount = 0;

	app.delegate.logic = logic;

	app.delegate.draw = draw;
}
static void logic(void)
{
	if (app.keyboard[SDL_SCANCODE_1])
	{
		app.keyboard[SDL_SCANCODE_1] = 0;

		awardMedal("key1");
	}

	if (app.keyboard[SDL_SCANCODE_RETURN])
	{
		app.keyboard[SDL_SCANCODE_RETURN] = 0;

		awardMedal("keyReturn");
	}

	if (app.keyboard[SDL_SCANCODE_SPACE])
	{
		app.keyboard[SDL_SCANCODE_SPACE] = 0;

		if (++spaceCount >= 3)
		{
			awardMedal("keySpace");
		}
	}
}
static void draw(void)
{
	Medal* m;
	int y, r, g, b;

	y = 200;

	for (m = game.medalsHead.next; m != NULL; m = m->next)
	{
		r = g = b = 128;

		if (m->awardDate > 0)
		{
			r = g = 255;
			b = 0;
		}

		drawText(m->title, 200, y, r, g, b, TEXT_RIGHT, 0);

		drawText(m->description, 240, y + 45, r, g, b, TEXT_RIGHT, 0);

		y += 135;
	}
}