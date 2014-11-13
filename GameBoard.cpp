#include "GameBoard.h"
#include <time.h>
#include <amp.h>
#include <iterator>
#include <algorithm>
#include <sstream>

int deg = 0.0f;

GameBoard::GameBoard(LPDIRECT3DDEVICE9 d3dDevice, int size, InputController* inputController, LPD3DXFONT font)
{
	m_d3dDevice = d3dDevice;
	m_inputController = inputController;
	m_boardSize = size;
	m_textureManager = new TextureManager();
	m_meshManager = new MeshManager(m_textureManager);
	m_cubeMesh = NULL;
	m_pointerMesh = NULL;


	m_font = font;
	
	NewGame();
}

GameBoard::~GameBoard()
{

	Release();
}



void GameBoard::Update()
{

	if(!m_gameOver)
	{
		/*
			check the input from the player, and move the pointer accordingly
		*/
		if (m_inputController->GetKeyUp(VK_UP))
			m_position.z += 1.0f;

		if (m_inputController->GetKeyUp(VK_DOWN))
			m_position.z -= 1.0f;

		if (m_inputController->GetKeyUp(VK_LEFT))
			m_position.x -= 1.0f;

		if (m_inputController->GetKeyUp(VK_RIGHT))
			m_position.x += 1.0f;

		// clamp the position so the player can't move outside of the game board.
		m_position.x = Clamp(m_position.x, 0.0f, m_boardSize - 1);
		m_position.z = Clamp(m_position.z, 0.0f, m_boardSize - 1);
	
		// try to place the 'next color'
		if(m_inputController->GetKeyUp(VK_SPACE))
		{
			if(ValidMove())
			{
				PlaceNextColor();
				GenerateNextColor();
			}

		}
	
		// update the y-position of the pointer with help of a sinus function for cool effects
		m_position.y = 0.5*sin(deg* D3DX_PI / 180) + 1;

		// increase the radians
		deg = (deg + 2) % 360;
	}
	else 
	{
		if(m_inputController->GetKeyUp(VK_RETURN))
		{
			NewGame();
			SetupAssets();
		}

	}
	
}

/*
	Check if the move the player wants to make is valid or not.
	Trying to change a cubes color which is already of the color 
	in 'next color' is not valid. It is also not valid to try and change
	color of a cube with the Color NONE

*/
bool GameBoard::ValidMove()
{
	Color c = m_gameBoard[m_position.x][m_position.z];
	if(c == NONE || c == m_nextColor )
		return false; 
	
	return true;
}

/*
	This function will be called whenever the player has changed the color of a
	cube that is valid. It will then check which adjacent cubes are of the same color
	as the next color. If there are 3 or more adjacent cubes whith the 'next color', they
	will be removed and the player's score increased.

	If the player has made a move which did not result in 3 or more adjacent cubes with the 
	'next color' the player has lost the game, and the game over screen will be set up correctyl.
*/
void GameBoard::PlaceNextColor()
{
	m_gameBoard[m_position.x][m_position.z] = m_nextColor;
	std::vector<D3DXVECTOR3> adjecent;
	adjecent.push_back(D3DXVECTOR3(m_position.x, 0, m_position.z));	// add the chosen cube position to the list
	FindAdjacentCubes(&adjecent, m_position, m_nextColor);			// find all other adjacent cubes with the 'next color'
	if(adjecent.size() > 3)			// greater than 3 since the currently chosen cube will always be in adjecent list
	{
		m_points += (100 * adjecent.size());			// increase player's score
		UpdateScoreString();							// update the score string

		// iterate over all adjacent cubes and set their color to NONE so they won't be rendered no more
		std::vector<D3DXVECTOR3>::iterator iter;
		for(iter = adjecent.begin(); iter != adjecent.end(); ++iter)
		{
			m_gameBoard[(*iter).x][(*iter).z] = NONE;
		}
	}
	else 
	{
		// the player has lost the game
		m_gameOver = true;
		SetupGameOverScreen();
	}
	
}
void GameBoard::FindAdjacentCubes(std::vector<D3DXVECTOR3>* adjecent, D3DXVECTOR3 startPos, Color color)
{
	std::vector<D3DXVECTOR3> newlyFound;
	newlyFound.clear();

	
	// create the positions of the adjecent cubes to the currentyl evaluated cube at startPos
	D3DXVECTOR3 above(startPos.x, 0, Clamp(startPos.z + 1, 0, m_boardSize - 1));	
	D3DXVECTOR3 below(startPos.x, 0, Clamp(startPos.z - 1, 0, m_boardSize - 1));

	D3DXVECTOR3 right(Clamp(startPos.x + 1, 0, m_boardSize - 1), 0, startPos.z);
	D3DXVECTOR3 left(Clamp(startPos.x - 1, 0, m_boardSize - 1), 0, startPos.z);

	// add the all to the newlyFound vector
	newlyFound.push_back(above);
	newlyFound.push_back(below);
	newlyFound.push_back(right);
	newlyFound.push_back(left);

	int i = 0;
	// Loop through newlyFound to se which ones are actually new to us.
	while(i < newlyFound.size()) 
	{
		D3DXVECTOR3 cubePos = newlyFound[i];
		Color cubeColor = m_gameBoard[cubePos.x][cubePos.z];
		if(std::find(adjecent->begin(), adjecent->end(), cubePos) != adjecent->end() || cubeColor != color)
		{
			// the cubePos has either already been found before or it does not have the correct color
			// remove it from the newlyFound list
			newlyFound.erase(newlyFound.begin() + i);
		}
		else
		{
			// we have found a new cube that is not already found and has the correct color
			// save it to the list of adjecent cubes with correct color
			adjecent->push_back(cubePos);
			
			// if we remove an element from newlyFound list we should not increment since the size of the vector
			// will shrink and we will point at the next element. But if we have not removed an element. We should increment i by 1
			i++;		
		}
	
	}
	
	// for all the newly found adjacent cubes find their adjacent cubes with Color color
	for(std::vector<D3DXVECTOR3>::iterator posIter = newlyFound.begin(); posIter != newlyFound.end(); ++posIter)
	{
		FindAdjacentCubes(adjecent, *posIter, color);
	}

}

void GameBoard::Render()
{
	// if game over draw game over screen, otherwise render the game board with hud and everything
	if(m_gameOver)
	{
		DrawGameOverScreen();
	}
	else
	{
		RenderGameBoard();
		RenderPointer();
		DrawNextColor();
		DrawText(m_scoreString, &m_scoreRect);
	}

}



void GameBoard::RenderGameBoard()
{
	D3DXMATRIX World;
	/*
		Loop through the game board matrix and render all the cubes which have a color
		don't render the ones that have been removed from the game board whit the Color NONE
		
		All cubes are of unit size and then we can draw them along side each other based
		on their position within the game board matrix.

	*/
	for(int col = 0; col < m_gameBoard.size(); col++)
	{
		for( int row = 0; row < m_gameBoard[col].size(); row++)
		{
			Color color = m_gameBoard[col][row];
			if(color != NONE)
			{
				const char* colorFilename = m_colors[color];	// get the file name of the cube color texture

				if(m_textureManager->Load(m_d3dDevice, colorFilename))
				{
					// set the world position of the cube to its position in the game board matrix
					D3DXMatrixTranslation(&World, col, 0.0f, row); 
					m_d3dDevice->SetTransform(D3DTS_WORLD, &World);
					
					// render the cube mesh with color texture specified by colorFilename
					m_cubeMesh->Render(m_d3dDevice, m_textureManager->GetTexture(colorFilename));		
				}
			}
		}
	}


}


void GameBoard::RenderPointer()
{
	// render the pointer to its current position along the game board
	D3DXMATRIX World;
	D3DXMatrixTranslation(&World, m_position.x, m_position.y, m_position.z);
	m_d3dDevice->SetTransform(D3DTS_WORLD, &World);
	m_pointerMesh->Render(m_d3dDevice);
}

void GameBoard::DrawNextColor()
{
	m_spriteObject->Begin(D3DXSPRITE_ALPHABLEND);

	if(m_textureManager->Load(m_d3dDevice, m_nextColorfileName.c_str()))
	{
		
		Texture* tex = m_textureManager->GetTexture(m_nextColorfileName.c_str());
		m_spriteObject->Draw(tex->GetTexture(), NULL, NULL, &m_nextColorPos, D3DCOLOR_COLORVALUE(1.0f,1.0f,1.0f,1.0f));

	}
	
	m_spriteObject->End();
}

void GameBoard::DrawText(std::string text, RECT* rect)
{
	D3DCOLOR fontColor = D3DCOLOR_ARGB(255,255,255,255); 
	m_font->DrawTextA(NULL, text.c_str(), -1, rect, DT_CENTER | DT_VCENTER , fontColor);
}

void GameBoard::DrawGameOverScreen()
{
	m_spriteObject->Begin(D3DXSPRITE_ALPHABLEND);
	if(m_textureManager->Load(m_d3dDevice, "GameOver.png"))
	{
		Texture* tex = m_textureManager->GetTexture("GameOver.png");
		m_spriteObject->Draw(tex->GetTexture(), NULL, NULL, &m_gameOverPos, D3DCOLOR_COLORVALUE(1.0f,1.0f,1.0f,1.0f));

	}

	m_spriteObject->End();

	DrawText(m_scoreString, &m_scoreRect);



}

void GameBoard::GenerateNextColor()
{
	 m_nextColor = (Color) (rand() % COLORS_SIZE);		// randomize next color
	const char* f = m_colors[m_nextColor];				// find next color in m_colors
	m_nextColorfileName = f;
	m_nextColorfileName.replace(0,4,"Next");			// replace the word "Cube" with "Next" to generate filename to sprite texture

}

bool GameBoard::SetupAssets()
{
	InitialiseColors();			// initialise the vector containing all possible colors.
	InitialiseGameBoard();		// initialise the game board matrix with random colors

	if(!InitialiseSprites())	// create the spriteObject, so we can render sprites
		return false;
	if(!InitialiseMeshes())		// initialise the cube and pointer mesh needed to render the game board.
		return false;

	return true;
}

void GameBoard::SetupGameOverScreen()
{
	if(m_textureManager->Load(m_d3dDevice, "GameOver.png"))
	{
		
		D3DDEVICE_CREATION_PARAMETERS cparams;
		RECT rect;

		m_d3dDevice->GetCreationParameters(&cparams);		// get creation parameters to get the screen height and width
		GetWindowRect(cparams.hFocusWindow, &rect);			// get the rectangle for the game window
		
		// calc screen height and width
		float screenWidth = rect.right - rect.left;
		float screenHeight = rect.bottom - rect.top;
		
		// get size of GameOver texture
		D3DXIMAGE_INFO textureInfo;
		D3DXGetImageInfoFromFile("GameOver.png", &textureInfo);

		// center the texture on the screen
		m_gameOverPos.x = (screenWidth/2 - textureInfo.Width/2);
		m_gameOverPos.y = (screenHeight/2 - textureInfo.Height/2);
		m_gameOverPos.z = 1.0f;
		
		// update the score rectangle to be below the game over texture
		m_scoreRect.top= m_gameOverPos.y + textureInfo.Height;
		m_scoreRect.left = m_gameOverPos.x;
		m_scoreRect.bottom = m_scoreRect.top + 100;
		m_scoreRect.right = m_scoreRect.left + textureInfo.Width;

		//std::string gameOverString;
		//gameOverString = m_scoreString;
		m_scoreString.append("\n RETURN = New Game");


	}
}

bool GameBoard::InitialiseMeshes()
{
	if(!m_meshManager->Load(m_d3dDevice, "Cube.x"))
		return false;
	m_cubeMesh = m_meshManager->GetMesh("Cube.x");

	if(!m_meshManager->Load(m_d3dDevice, "Pointer.x"))
		return false;

	m_pointerMesh = m_meshManager->GetMesh("Pointer.x");
	
	return true;
}
bool GameBoard::InitialiseSprites()
{
	GenerateNextColor();
	//Create a sprite object to draw the button with
	if(FAILED(D3DXCreateSprite(m_d3dDevice, &m_spriteObject)))
	{
		return false;
	}
	return true;
}
	
void GameBoard::InitialiseGameBoard() 
{
	m_gameBoard.clear();				// clear the game board matrix
	m_gameBoard.resize(m_boardSize);	// set the correct size 
	for(int column = 0; column < m_boardSize; column++)
	{
		m_gameBoard[column].clear();
		m_gameBoard[column].resize(m_boardSize);
		for(int row = 0; row < m_boardSize; row++)
		{
			int color = rand() % COLORS_SIZE;			// randomize a color from the Color enum
			m_gameBoard[column][row] = (Color) color;	// put color in game board matrix
		}
	}
}

void GameBoard::InitialiseColors()
{
	m_colors.clear();
	m_colors.resize(COLORS_SIZE);

	m_colors[BLACK] = "Cube-Black.png";
	m_colors[BLUE] = "Cube-Blue.png";
	m_colors[RED] = "Cube-Red.png";
	m_colors[GREEN] = "Cube-Green.png";

	m_colors[PURPLE] = "Cube-Purple.png";
	m_colors[YELLOW] = "Cube-Yellow.png";
	m_colors[WHITE] = "Cube-White.png";
	m_colors[ORANGE] = "Cube-Orange.png";

}

void GameBoard::UpdateScoreString()
{
	/*	this method is called whenever the player has increased his score.
		clear the string containing old data and concatenate the string 'score:' with
		the players current score
	*/
	m_scoreString.clear();
	m_scoreString = "score: ";
	std::ostringstream ss;
	ss << m_points;
	m_scoreString.append(ss.str());
}

void GameBoard::NewGame()
{
	m_points = 0;
	// center the player
	m_position.x = (m_boardSize % 2 == 0) ? m_boardSize/2 - 1 : (m_boardSize / 2 ) - 0.5;
	m_position.z = (m_boardSize % 2 == 0) ? m_boardSize/2 - 1 : (m_boardSize / 2 ) - 0.5;
	m_position.y = 1.0f;

	m_nextColorPos = D3DXVECTOR3(1.0f,1.0f,0.0f);
	
	m_scoreRect.top = m_nextColorPos.y + 128 ;
	m_scoreRect.bottom = m_scoreRect.top + 20;
	m_scoreRect.left = m_nextColorPos.x;
	m_scoreRect.right = m_scoreRect.left + 128;
	
	UpdateScoreString();
	srand(time(NULL));

	m_gameOver = false;
}


/*
	Helper method to clamp a position within the game board as to avoid index out of bound exception
*/

float GameBoard::Clamp(float val, float MIN, float MAX)
{
	
	return max(MIN, min(MAX, val));
}

void GameBoard::Release() 
{
	if(!m_colors.empty())
		m_colors.clear();

	if(!m_gameBoard.empty())
		m_gameBoard.clear();

	if(m_meshManager)
	{
		if(m_cubeMesh)
		{
			m_meshManager->ReleaseMesh(m_cubeMesh);
			m_cubeMesh = NULL;
		}
			
		if(m_pointerMesh)
		{
			m_meshManager->ReleaseMesh(m_pointerMesh);
			m_pointerMesh = NULL;
		}
		m_meshManager->Release();
		m_meshManager = NULL;
	}

	if(m_textureManager)
	{
		m_textureManager->Release();
		m_textureManager = NULL;
	}

	if(m_spriteObject)
	{
		m_spriteObject->Release();
		m_spriteObject = NULL;
	}
	if(m_d3dDevice)
	{
		m_d3dDevice->Release();
		m_d3dDevice = NULL;
	}
		
}

