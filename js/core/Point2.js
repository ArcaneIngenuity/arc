Disjunction.Core.Point2 = function(x, y)
{
	this.x = x || 0;
	this.y = y || 0;
	
	this.clone = function()
	{
		return new Disjunction.Core.Point2(this.x, this.y);
		 clone;
	}
};

Disjunction.Core.Point2.prototype.constructor = Disjunction.Core.Point2; //TODO remove?

if (disjunction.WINDOW_CLASSES) 
	window.Point2 = Disjunction.Core.Point2;