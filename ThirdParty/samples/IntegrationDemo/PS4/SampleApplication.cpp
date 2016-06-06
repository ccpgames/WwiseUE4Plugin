
#include "SampleApplication.h"

int PS4SampleApplication::initialize(void)
{
	int ret;
	(void)ret;

	int initflags = kFunctionFlagGraphics | kFunctionFlagSpriteRenderer | kFunctionFlagFios2;
	ret = initializeUtil(initflags, DISPLAY_WIDTH, DISPLAY_HEIGHT);
	AKASSERT(ret == SCE_OK);

	ssg::createSpriteRenderer(&m_sprite, getGraphicsContext());
	m_sprite->setRenderTargetSize(vecmath::Vector2(DISPLAY_WIDTH, DISPLAY_HEIGHT));
	m_camera.reset();

	return SCE_OK;
}

int PS4SampleApplication::update(void)
{
	int ret;

	ret = updateUtil();
	assert(ret == SCE_OK);
	
	return SCE_OK;
}	

void PS4SampleApplication::BeginScene(void)
{
	getGraphicsContext()->beginScene(getGraphicsContext()->getNextRenderTarget(),
									 getGraphicsContext()->getDepthStencilSurface());

	getGraphicsContext()->clearRenderTarget(0x00000000);

	getGraphicsContext()->setDepthWriteEnable(true);
	getGraphicsContext()->setDepthFunc(sce::SampleUtil::Graphics::kDepthFuncLessEqual);
}

void PS4SampleApplication::EndScene(void)
{
	getGraphicsContext()->endScene();
	getGraphicsContext()->flip( 1);
}

void PS4SampleApplication::render(void)
{
}

void PS4SampleApplication::DrawString(vecmath::Vector2 pos, vecmath::Vector4 color, const char* msg)
{
	char strCopy[256];
	const char* stringStart = msg;
	int stringLen = strlen(msg);
	const char* stringEnd = NULL;

	do
	{
		stringEnd = strchr(stringStart+1,'\n');
		stringLen = (stringEnd != NULL) ? (stringEnd - stringStart) : (strlen(stringStart));
		strncpy(strCopy, stringStart, stringLen);
		strCopy[stringLen] = '\0';
		m_sprite->drawDebugStringf(getGraphicsContext(), pos, 0.045, color, strCopy);
		pos.setY( pos.getY() + 0.035 );

		if (stringEnd != NULL)
			stringStart = stringEnd + 1;

	}while (stringEnd != NULL);
}

int PS4SampleApplication::finalize(void)
{
	int ret;
	(void)ret;

	ret = finalizeUtil();
	assert(ret == SCE_OK);
	return SCE_OK;
}