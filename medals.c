#include "common.h"

void initMedals(void)
{
	loadMedals();

	medalTextures[MEDAL_BRONZE] = getAtlasImage("gfx/bronzeMedal.png", 1);
	medalTextures[MEDAL_SILVER] = getAtlasImage("gfx/silverMedal.png", 1);
	medalTextures[MEDAL_GOLD] = getAtlasImage("gfx/goldMedal.png", 1);
	medalTextures[MEDAL_RUBY] = getAtlasImage("gfx/rubyMedal.png", 1);

	notifyOrder = 1;

	resetAlert();
}
void awardMedal(char* id)
{
	Medal* m;
	int numRemaining, hasRuby;

	hasRuby = numRemaining = 0;

	for (m = game.medalsHead.next; m != NULL; m = m->next)
	{
		if (m->awardDate == 0 && strcmp(m->id, id) == 0)
		{
			m->awardDate = time(NULL);

			m->notify = SDL_GetTicks() + notifyOrder++;

			SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "Awarding medal '%s'", m->id);
		}

		if (m->awardDate == 0)
		{
			numRemaining++;
		}

		if (strcmp(m->id, "ruby") == 0)
		{
			hasRuby = 1;
		}
	}

	if (numRemaining == 1 && hasRuby)
	{
		awardMedal("ruby");
	}
}
void doMedalAlerts(void)
{
	if (alertMedal == NULL)
	{
		nextAlert();
	}
	else
	{
		alertRect.x = MAX(alertRect.x - app.deltaTime * 24, SCREEN_WIDTH - alertRect.w);

		alertTimer -= app.deltaTime;

		if (alertTimer <= 0)
		{
			alertMedal->notify = 0;

			resetAlert();
		}
	}
}
static void nextAlert(void)
{
	int i, w, h, l;
	Medal* m;

	for (m = game.medalsHead.next; m != NULL; m = m->next)
	{
		if (m->notify != 0)
		{
			if (alertMedal == NULL || m->notify < alertMedal->notify)
			{
				alertMedal = m;
			}
		}
	}

	if (alertMedal != NULL)
	{
		playSound(SND_MEDAL, -1);

		app.fontScale = 1.0;

		calcTextDimensions(alertMedal->title, &w, &h);

		w += 125;

		alertRect.w = MAX(w, alertRect.w);

		memset(abbrevDescription, 0, sizeof(abbrevDescription));

		l = strlen(alertMedal->description);

		for (i = 0; i < l; i++)
		{
			calcTextDimensions(abbrevDescription, &w, &h);

			if (w < alertRect.w)
			{
				strncat(abbrevDescription, &alertMedal->description[i], 1);
			}
			else
			{
				strcat(abbrevDescription, "...");
				break;
			}
		}
	}
}
static void resetAlert(void)
{
	alertTimer = FPS * 3;
	alertMedal = NULL;

	alertRect.x = SCREEN_WIDTH;
	alertRect.y = 10;
	alertRect.w = 475;
	alertRect.h = 90;
}
void drawMedalAlert(void)
{
	if (alertMedal != NULL)
	{
		drawRect(alertRect.x, alertRect.y, alertRect.w, alertRect.h, 0, 0, 0, 255);
		drawOutlineRect(alertRect.x, alertRect.y, alertRect.w, alertRect.h, 255, 255, 255, 255);

		blitAtlasImage(medalTextures[alertMedal->type], alertRect.x + 45, alertRect.y + 45, 1, SDL_FLIP_NONE);

		drawText(alertMedal->title, alertRect.x + 90, alertRect.y + 5, 255, 255, 255, TEXT_RIGHT, 0);

		app.fontScale = 0.7;

		drawText(abbrevDescription, alertRect.x + 90, alertRect.y + 45, 255, 255, 255, TEXT_RIGHT, 0);

		app.fontScale = 1.0;
	}
}
static void loadMedals(void)
{
	Medal* m, * prev;
	cJSON* root, * node;
	char* text;

	prev = &game.medalsHead;

	text = readFile("data/medals/medals.json");

	root = cJSON_Parse(text);

	for (node = root->child; node != NULL; node = node->next)
	{
		m = malloc(sizeof(Medal));
		memset(m, 0, sizeof(Medal));
		prev->next = m;
		prev = m;

		m->type = lookup(cJSON_GetObjectItem(node, "type")->valuestring);
		STRCPY(m->id, cJSON_GetObjectItem(node, "id")->valuestring);
		STRCPY(m->title, cJSON_GetObjectItem(node, "title")->valuestring);
		STRCPY(m->description, cJSON_GetObjectItem(node, "description")->valuestring);

		if (cJSON_GetObjectItem(node, "hidden"))
		{
			m->hidden = 1;
		}
	}

	cJSON_Delete(root);

	free(text);
}