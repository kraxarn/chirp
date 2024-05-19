#include "chirp/collision.hpp"

#include "chirp/internal/utils.hpp"

auto chirp::collision::check(const rectangle<float> &rec1, const rectangle<float> &rec2) -> bool
{
	const auto r_rec1 = internal::utils::to_r_rectangle(rec1);
	const auto r_rec2 = internal::utils::to_r_rectangle(rec2);

	return CheckCollisionRecs(r_rec1, r_rec2);
}
