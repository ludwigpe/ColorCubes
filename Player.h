#ifndef PLAYER_H
#define PLAYER_H

#include <d3dx9.h>
#include "Mesh.h"

class Player
{
private:
	D3DXVECTOR3 m_position;
	float m_points;
	Mesh* m_playerMesh;

public:
	Player(Mesh* mesh);
	~Player();
	void AddPoints(float p);
	float GetPoints() {return m_points;}
	D3DXVECTOR3 GetPosition() { return m_position; }

};


#endif