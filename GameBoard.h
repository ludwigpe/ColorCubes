#ifndef GAMEBOARD_H_
#define GAMEBOARD_H_
#include <d3dx9.h>

#include "InputController.h"
#include "Mesh.h"
#include "MeshManager.h"

#include <vector>
enum Color
{
	BLACK = 0,
	BLUE,
	RED,
	GREEN,
	PURPLE,
	YELLOW,
	WHITE,
	ORANGE,
	COLORS_SIZE,
	NONE
};

class GameBoard 
{
private:
#pragma region vars
	LPDIRECT3DDEVICE9 m_d3dDevice;

	Mesh* m_cubeMesh;					// mesh for cube
	Mesh* m_pointerMesh;				// mesh for pointer
	MeshManager* m_meshManager;			// mesh manager for not loading mesher more than needed
	TextureManager* m_textureManager;	// texture manager for not loading textures more than needed
	
	LPD3DXSPRITE m_spriteObject;		// sprite object to draw sprites
	D3DXVECTOR3 m_position;				// position of the pointer
	D3DXVECTOR3 m_nextColorPos;			// position for where the next color sprite should be drawn
	D3DXVECTOR3 m_gameOverPos;			// position for where the game over sprite should be drawn
	
	float m_points;						// the player's points
	float Clamp(float val, float MIN, float MAX);		// function to clamp a value betwee min and max
	InputController* m_inputController;					// input controller for reading input from the player
	
	std::vector<const char*> m_colors;					// vector containing the filenames for the different cube color textures
	std::vector<std::vector<Color>> m_gameBoard;		// a matrix with Color values for the different cubes that make up the game board
	std::string m_nextColorfileName;					// a string with the 'next color' texture file name

	Color m_nextColor;									// current Color of 'next color'
	LPD3DXFONT m_font;									// font object for printing playe's score
	RECT m_scoreRect;									// rectangle to show the score
	std::string m_scoreString;							// a string representation of the player's score
	bool m_gameOver;									// boolean to see if the game is over or not, initially false.

#pragma endregion
#pragma region funcs

	/*
	All functions are described in more detail in the cpp file
	*/
	void GenerateNextColor();
	bool InitialiseMeshes();
	void InitialiseColors();
	bool InitialiseSprites();
	void SetupGameOverScreen();
	void DrawNextColor();
	void DrawScore();
	void DrawText(std::string text, RECT* rect);
	void DrawGameOverScreen();
	void RenderGameBoard();
	void RenderPointer();
	int m_boardSize;
	bool ValidMove();
	void PlaceNextColor();
	void UpdateScoreString();
	void FindAdjacentCubes(std::vector<D3DXVECTOR3>* adjecent, D3DXVECTOR3 startPos, Color color);
	std::vector<D3DXVECTOR3> GetAdjacentOfColor( D3DXVECTOR3 startPos, Color color);

	void NewGame();
#pragma endregion
public:
	GameBoard(LPDIRECT3DDEVICE9 d3dDevice, int size, InputController* inputController,LPD3DXFONT font);
	~GameBoard();
	void InitialiseGameBoard();
	
	void Update();
	void Render();
	bool SetupAssets();
	bool GameOver() {return m_gameOver;}
	void Release();
};

#endif