#include "FadingBirdSprite.h"

FadingBirdSprite::FadingBirdSprite(ID3D11Resource* spriteSheetResource, ID3D11ShaderResourceView* spriteSheet, 
		int numColumns, int numRows, int anAnimationFrameRate, XMFLOAT2 aLocation, XMFLOAT2 aVelocity) 
		: Sprite(spriteSheetResource, spriteSheet, numColumns, numRows, anAnimationFrameRate, aLocation, aVelocity)
{
	fadingOut = true;
}


FadingBirdSprite::~FadingBirdSprite(void)
{
}

void FadingBirdSprite::Update(float gameTime, RECT clientBounds)
{
	/*clientBounds represents the area the sprite character must remain within
        * typically the game window
        */

    //modify the location based on current speed
    location = XMFLOAT2(location.x + speed.x, location.y + speed.y);
	
    //move to the next frame on the sprite sheet
	int elapsedMilliseconds = gameTime*1000;
	//move to the next frame on the sprite sheet
	if (elapsedMilliseconds - lastRecordedTime > 1000 / animationFrameRate )
    {
		int rowsPlusOne = rows + 1;

		if (currentColumn == (columns - 1) && currentRow == rows) fadingOut = false;
		else if (currentColumn == 0 && currentRow == 0) fadingOut = true;

		if (fadingOut)
		{
			currentColumn = (currentColumn + 1) % columns;
            if (currentColumn == 0) currentRow = (currentRow + 1) % (rows + 1);
		}
		// Can't just use "modulus" here because % isn't really the modulus operator
		// http://stackoverflow.com/questions/12276675/modulus-with-negative-numbers-in-c
		else {
			currentColumn = ((currentColumn - 1)  % columns + columns) % columns;
			if (currentColumn == 0) currentRow = ((currentRow - 1) % rows + rows) % columns;
		}

        lastRecordedTime = elapsedMilliseconds;
    }

    //keep the sprite within the client bounds
	float clientWidth = clientBounds.right - clientBounds.left;
	float clientHeight = clientBounds.top - clientBounds.bottom;

    if (location.x > clientWidth - frameWidth) speed.x = -speed.x;
    if (location.x < 0) speed.x = -speed.x;
    if (location.y > clientHeight - frameHeight) speed.y = -speed.y;
    if (location.y < 0) speed.y = -speed.y;
}

void FadingBirdSprite::Draw(SpriteBatch* spriteBatch, float gameTime)
{
	RECT sourceFrame = { 
		currentColumn * frameWidth, 
		currentRow * frameHeight, 
		currentColumn * frameWidth + frameWidth, 
		currentRow * frameHeight + frameHeight
	};
	SpriteEffects directionFlip = SpriteEffects_None;
    if (speed.x < 0) directionFlip = SpriteEffects_FlipHorizontally;

	spriteBatch->Draw( spriteSheet, location, &sourceFrame, Colors::White, rotation, rotationOrigin, 1.0f, directionFlip, 0.0f );
}