#include "WorldTransform.h"
#include "newMath.h"

void WorldTransform::UpdateMatrix() {
	matWorld_ = Math::MakeAffineMatrix(scale_, rotation_, translation_);

	if (parent_) {
		matWorld_ = Math::Multiply(matWorld_, parent_->matWorld_);
	}

	TransferMatrix();
}