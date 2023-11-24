#include "ParticleEmitter.h"

void ParticleEmitter::Initialize(const Vector3& translation, const Vector3& velocity, const Vector4& color, float frequency, float particleLifeTime, float emitterLifeTime) {
	//パーティクルの発生位置の初期化
	popTranslation_ = translation;
	//初速度の初期化
	popVelocity_ = { velocity,velocity };
	//色の初期化
	popColor_ = { color,color };
	//発生頻度の初期化
	popFrequency_ = frequency;
	//パーティクルの寿命を設定
	popLifeTime_ = { particleLifeTime,particleLifeTime };
	//エミッターの寿命を設定
	deleteTime_ = emitterLifeTime;
}

void ParticleEmitter::Update() {
	//パーティクルを生成
	const float kDeltaTime = 1.0f / 60.0f;
	frequencyTime_ += kDeltaTime;
	if (popFrequency_ <= frequencyTime_) {
		for (uint32_t index = 0; index < popCount_; ++index) {
			ParticleEmitter::Pop();
		}
		frequencyTime_ -= popFrequency_;
	}

	//パーティクルの更新
	for (std::list<std::unique_ptr<Particle>>::iterator particleIterator = particles_.begin(); particleIterator != particles_.end(); ++particleIterator) {
		particleIterator->get()->Update();
	}

	//エミッターの死亡フラグを立てる
	deleteTimer_ += kDeltaTime;
	if (deleteTimer_ > deleteTime_) {
		isDead_ = true;
	}
}

void ParticleEmitter::Pop() {
	//座標
	Vector3 translation = {
		popTranslation_.x + RandomGenerator::GetRandomFloat(popArea_.min.x,popArea_.max.x),
		popTranslation_.y + RandomGenerator::GetRandomFloat(popArea_.min.y,popArea_.max.y),
		popTranslation_.z + RandomGenerator::GetRandomFloat(popArea_.min.z,popArea_.max.z)
	};

	//回転
	Vector3 rotation = {
		RandomGenerator::GetRandomFloat(popRotation_.min.x,popRotation_.max.x),
		RandomGenerator::GetRandomFloat(popRotation_.min.y,popRotation_.max.y),
		RandomGenerator::GetRandomFloat(popRotation_.min.z,popRotation_.max.z)
	};

	//スケール
	Vector3 scale = {
		RandomGenerator::GetRandomFloat(popScale_.min.x,popScale_.max.x),
		RandomGenerator::GetRandomFloat(popScale_.min.y,popScale_.max.y),
		RandomGenerator::GetRandomFloat(popScale_.min.z,popScale_.max.z)
	};

	//方位角
	float azimuth = { RandomGenerator::GetRandomFloat(popAzimuth.min,popAzimuth.max) };
	float azimuthRadian = azimuth * float(std::numbers::pi / 180.0f);

	//仰角
	float elevation = { RandomGenerator::GetRandomFloat(popElevation.min,popElevation.max) };
	float elevationRadian = elevation * float(std::numbers::pi / 180.0f);

	//速度
	Vector3 velocity = {
		RandomGenerator::GetRandomFloat(popVelocity_.min.x,popVelocity_.max.x) * std::cos(elevationRadian) * std::cos(azimuthRadian),
		RandomGenerator::GetRandomFloat(popVelocity_.min.y,popVelocity_.max.y) * std::cos(elevationRadian) * std::sin(azimuthRadian),
		RandomGenerator::GetRandomFloat(popVelocity_.min.z,popVelocity_.max.z) * std::sin(elevationRadian)
	};

	//色
	Vector4 color = {
		RandomGenerator::GetRandomFloat(popColor_.min.x,popColor_.max.x),
		RandomGenerator::GetRandomFloat(popColor_.min.y,popColor_.max.y),
		RandomGenerator::GetRandomFloat(popColor_.min.z,popColor_.max.z),
		RandomGenerator::GetRandomFloat(popColor_.min.w,popColor_.max.w)
	};

	//寿命
	float lifeTime = RandomGenerator::GetRandomFloat(popLifeTime_.min, popLifeTime_.max);

	//パーティクルの生成
	Particle* particle = new Particle();
	particle->Initialize(translation, rotation, scale, velocity, color, lifeTime);
	//パーティクルをリストに追加
	particles_.push_back(std::unique_ptr<Particle>(particle));
}