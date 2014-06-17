Math.degToRad = function(angleDeg)
{
	return(angleDeg*Math.PI/180);
}

Math.radToDeg = function(angleRad)
{
	return(angleRad*180/Math.PI);
}

Math.clamp = function(val, min, max)
{
	return Math.max(min,Math.min(max, val));
}