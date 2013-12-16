#include "Sprite.h"

Sprite::Sprite(ID3D11Resource* aSpriteSheetResource, ID3D11ShaderResourceView* aSpriteSheet, int numColumns, int numRows, int anAnimationFrameRate, XMFLOAT2 aLocation, XMFLOAT2 aVelocity)
{

	spriteSheet = aSpriteSheet;

	// Get height and width of spritesheet
	auto txt = reinterpret_cast<ID3D11Texture2D*>( aSpriteSheetResource );
	D3D11_TEXTURE2D_DESC desc;
	txt->GetDesc( &desc );

	float spriteSheetWidth = desc.Width;
	float spriteSheetHeight = desc.Height;

    currentRow = 0;
    currentColumn = 0;
    rows = numRows;
    columns = numColumns;
    frameWidth = spriteSheetWidth / columns;
    frameHeight = spriteSheetHeight / rows;
    animationFrameRate = anAnimationFrameRate;
    lastRecordedTime = 0;

    location = aLocation;
    speed = aVelocity;

    rotation = 0;
    rotationOrigin = XMFLOAT2(0, 0);
}

void Sprite::Update(float gameTime, RECT clientBounds)
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
		currentColumn = (currentColumn + 1) % columns;
        if (currentColumn == 0) currentRow = (currentRow + 1) % rows;
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

void Sprite::Draw(SpriteBatch* spriteBatch, float gameTime)
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