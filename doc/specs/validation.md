Algorithm for finding the position:

pos = 0
FOR cardinality
    match = test_match_children
	if FOUND
		return FOUND
	if MATCH
		pos = child.pos
		matches++
		if matches >= max_card
			return SKIP_NEXT
		else
			continue
	else { NO MATCH }
		if min_card > matches
			return FAIL
		else
			return SKIP_NEXT
	end
end
