#include <math.h>
#include "ShootingScene.h"

void ShootingScene::GenerateDisplayList()
{
	int firstlist;

	firstlist=glGenLists(5);

	Box1_list = firstlist;
	Box2_list = firstlist + 1;
	Box3_list = firstlist + 2;
	Box4_list = firstlist + 3;
	Chamber_list = firstlist + 4;

	//List for Box 1.
	glNewList(Box1_list, GL_COMPILE);
		pBox1->draw();
	glEndList();

	//List for Box 2.
	glNewList(Box2_list, GL_COMPILE);
		pBox2->draw();
	glEndList();

	//List for Box 3.
	glNewList(Box3_list, GL_COMPILE);
		pBox3->draw();
	glEndList();

	//List for Box 4.
	glNewList(Box4_list, GL_COMPILE);
		pBox4->draw();
	glEndList();

	//List for chamber.
	glNewList(Chamber_list, GL_COMPILE);
		pChamber->draw();
	glEndList();
};