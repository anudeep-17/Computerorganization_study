//header of a Map addresstoval.c
#ifndef addresstovaluedict
#define addresstovaluedict

#define MaxMapsize 500 //Max map elements

struct addresstoval
{
	char address[30];
	int value;
};

struct Map
{
	struct addresstoval Mapitems[MaxMapsize];
	int currentsize;
};

//method declarations
void initializeMap(struct Map* map);
void insert(struct Map* map, const char* key, int value);
int find(struct Map* map, const char* key);
void printMap(struct Map *map);

#endif