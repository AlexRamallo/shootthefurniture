//
//  easing.h [modified by alex]
//
//  Copyright (c) 2011, Auerhaus Development, LLC
//
//  This program is free software. It comes without any warranty, to
//  the extent permitted by applicable law. You can redistribute it
//  and/or modify it under the terms of the Do What The Fuck You Want
//  To Public License, Version 2, as published by Sam Hocevar. See
//  http://sam.zoy.org/wtfpl/COPYING for more details.
//
#pragma once
typedef float (*AHEasingFunction)(float);

namespace Ease {

	// Linear interpolation (no easing)
	float LinearInterpolation(float p);

	// Quadratic easing; p^2
	float QuadraticEaseIn(float p);
	float QuadraticEaseOut(float p);
	float QuadraticEaseInOut(float p);

	// Cubic easing; p^3
	float CubicEaseIn(float p);
	float CubicEaseOut(float p);
	float CubicEaseInOut(float p);

	// Quartic easing; p^4
	float QuarticEaseIn(float p);
	float QuarticEaseOut(float p);
	float QuarticEaseInOut(float p);

	// Quintic easing; p^5
	float QuinticEaseIn(float p);
	float QuinticEaseOut(float p);
	float QuinticEaseInOut(float p);

	// Sine wave easing; sin(p * PI/2)
	float SineEaseIn(float p);
	float SineEaseOut(float p);
	float SineEaseInOut(float p);

	// Circular easing; sqrt(1 - p^2)
	float CircularEaseIn(float p);
	float CircularEaseOut(float p);
	float CircularEaseInOut(float p);

	// Exponential easing, base 2
	float ExponentialEaseIn(float p);
	float ExponentialEaseOut(float p);
	float ExponentialEaseInOut(float p);

	// Exponentially-damped sine wave easing
	float ElasticEaseIn(float p);
	float ElasticEaseOut(float p);
	float ElasticEaseInOut(float p);

	// Overshooting cubic easing;
	float BackEaseIn(float p);
	float BackEaseOut(float p);
	float BackEaseInOut(float p);

	// Exponentially-decaying bounce easing
	float BounceEaseIn(float p);
	float BounceEaseOut(float p);
	float BounceEaseInOut(float p);

};