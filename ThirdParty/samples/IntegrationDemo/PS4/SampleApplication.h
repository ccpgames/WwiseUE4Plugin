
#pragma once

#include <AK/SoundEngine/Common/AkTypes.h>

#include <sample_code/common/include/sampleutil.h>
#include <sample_code/system/api_sample_util/game_camera.h>
#include <sampleutil/input.h>

namespace vecmath = sce::Vectormath::Simd::Aos;
namespace ssg = sce::SampleUtil::Graphics;
namespace ssi = sce::SampleUtil::Input;

class PS4SampleApplication : public sce::SampleUtil::SampleSkeleton
{
private:
	ssg::SpriteRenderer* m_sprite;

	static const int DISPLAY_WIDTH  = 1280;
	static const int DISPLAY_HEIGHT = 720;
	Camera m_camera;

public:
	virtual int initialize(void);
	void BeginScene(void);
	void EndScene(void);
	virtual void render(void);
	virtual int update(void);
	void DrawString(vecmath::Vector2 pos, vecmath::Vector4 color, const char* msg);
	virtual int finalize(void);
};

