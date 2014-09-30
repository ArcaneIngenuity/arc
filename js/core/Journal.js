//TODO as constructor length doesn't work, consider benefit of alternative approach for Journals http://www.bennadel.com/blog/2292-extending-javascript-arrays-while-keeping-native-bracket-notation-functionality.htm 
Disjunction.Core.Journal = function(length)
{
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

/**
 *	Set the Journal's older values.
 */
Disjunction.Core.Journal.prototype.update = function()
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
