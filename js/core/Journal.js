//TODO as constructor length doesn't work, consider benefit of alternative approach for Journals http://www.bennadel.com/blog/2292-extending-javascript-arrays-while-keeping-native-bracket-notation-functionality.htm 
Disjunction.Core.Journal = function(length, regulate)
{
	this.regulate = regulate;
	this.locked = false;
	
	if (!length)
		length = 2;
		
	Array.call(this, length); //doesn't work for setting length
	
	//HACK for length
	for (var i = 0; i < length; i++)
	{
		this.push(null);
	}
};
Disjunction.Core.Journal.prototype = new Array;

Disjunction.Core.Journal.prototype.update = function(value)
{
	if (this.locked)
		throw "Error: Journals set to lockOnUpdate may only be updated once before locks clear (on App.update()).";
	else
		if (this.regulate)
			this.locked = true;

	var length = this.length;
	
	//set all later values by shifting forward
	for (var i = 1; i < length - 1; i++)
	{
		this[i+1] = this[i];
	}
	this[0] = value;
}

Disjunction.Core.Journal.prototype.unlock = function() //framework use only!
{
	this.locked = false;
}

/**
 *	Set the Journal's older values.
 */
Disjunction.Core.Journal.prototype.postUpdate = function()
{
	var length = this.length;
	//console.log('jl', length);
	//set all later values by shifting forward
	for (var i = 0; i < length - 1; i++)
	{
		this[i+1] = this[i];
		//console.log('i', this[i+1], this[i]);
	}
}

/**
 *	Set the Journal's current value.
 */
Disjunction.Core.Journal.prototype.set = function(value)
{
	this[0] = value;
}

/**
 *	Set all the Journal's values.
 */
Disjunction.Core.Journal.prototype.setAll = function(value)
{
	for (var i = 0; i < length; i++)
		this[i] = value;
}

/**
 *	Check whether the Journal has changed between current value and most recent historical value.
 */
Disjunction.Core.Journal.prototype.changed = function()
{
	return this[0] != this[1];
}

/**
 *	Check whether the Journal has changed between two designated indices.
 */
Disjunction.Core.Journal.prototype.changedBetween = function(i, j)
{
	if (j === undefined)
		j = i + 1; //we will check against the immediately preceding value
	return this[i] != this[j];
}

/**
 *	Get the current value.
 */
Disjunction.Core.Journal.prototype.current = function()
{
	return this[0];
}

/**
 *	Get the last value.
 */
Disjunction.Core.Journal.prototype.last = function()
{
	return this[1];
}

