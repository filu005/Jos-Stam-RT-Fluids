#pragma once

// screen space constants
namespace c
{
	auto const screenWidth = 800u;
	auto const screenHeight = 600u;

	const float aspectRatio = static_cast<float>(screenWidth) / static_cast<float>(screenHeight);
	const float viewHeight = 1.2f;
	const float viewWidth = aspectRatio * viewHeight;// 1.6
}

// simulation constants
namespace c
{
	auto const dt = 1.0f / 60.0f;

	auto const diffusion = 0.5f;
}

// grid constants
namespace c
{
	// tylko potegi 2 (bo powstaja niedokladnosci przy dzieleniu m.in. przy dx/dy/dz)
	auto const K = 128u;
	auto const L = 128u;
	auto const KL = K * L;

	auto const ymin = -1.0f, xmin = -1.0f;
	auto const ymax = 1.0f, xmax = 1.0f;
	const float dx = (xmax - xmin) / static_cast<float>(K);
	const float dy = (ymax - ymin) / static_cast<float>(L);
}