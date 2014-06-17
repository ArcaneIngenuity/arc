

function entityManager_setActive(entityManager, e, cs, value)
{
	if (cs instanceof Array)
	{
		if (cs.length > BIT_LIMIT) cs.length = BIT_LIMIT;
		for (var ci in cs) //BIT_LIMIT bits per boolean
		{
			var c = cs[ci];
			entityManager_setActiveOne(entityManager, e, c, value);
		}
	}
	else
	{
		var c = cs;
		entityManager_setActiveOne(entityManager, e, c, value);
	}	
	console.log('after set all', Number(entityManager.active[e]).toString(2));
}
//activate a particular entity's particular component. value must be a bool.
function entityManager_setActiveOne(entityManager, e, c, value)
{	
	var invMask = 0xFFFFFFF;
	var position = 1 << c;
	console.log('value', value);
	console.log('c', c);
	console.log('pos', Number(position).toString(2));
	//create the mask
	var mask = invMask ^ position;
	console.log('mask', Number(mask).toString(2));
	//apply the mask
	var result = entityManager.active[e] & mask;
	console.log('result', Number(result).toString(2));
	//replace the position that the mask blanked out, with the actual value
	result |= (value << c); 
	
	entityManager.active[e] = result;
	console.log('after set one', Number(entityManager.active[e]).toString(2));
}