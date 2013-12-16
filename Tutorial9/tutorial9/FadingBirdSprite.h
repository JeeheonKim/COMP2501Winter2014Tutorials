#pragma once
#include "sprite.h"
class FadingBirdSprite :
	public Sprite
{
public:
	FadingBirdSprite(ID3D11Resource* spriteSheetResource, ID3D11ShaderResourceView* spriteSheet, 
		int numColumns, int numRows, int anAnimationFrameRate, XMFLOAT2 aLocation, XMFLOAT2 aVelocity);
	~FadingBirdSprite(void);
	
public:
	void Update(float gameTime, RECT clientBounds);
	void Draw(SpriteBatch* spriteBatch, float gameTime);

private:
	bool fadingOut;
};

