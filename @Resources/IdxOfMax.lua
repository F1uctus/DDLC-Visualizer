function Initialize()
	last_mx1idx = 0
	last_mx2idx = 0
	last_mx3idx = 0
	last_mx4idx = 0
end

function Update()
	b0 = SKIN:GetMeasure('MeasureBand0'):GetValue()
	b1 = SKIN:GetMeasure('MeasureBand1'):GetValue()
	b2 = SKIN:GetMeasure('MeasureBand2'):GetValue()
	b3 = SKIN:GetMeasure('MeasureBand3'):GetValue()
	b4 = SKIN:GetMeasure('MeasureBand4'):GetValue()
	return MaxIdx(b0, b1, b2, b3, b4)
end

function indexsort(tbl)
	local idx = {}
	for i = 1, #tbl do idx[i] = i end -- build a table of indexes
	-- sort the indexes, but use the values as the sorting criteria
	table.sort(idx, function(a, b) return tbl[a] > tbl[b] end)
	-- return the sorted indexes
	return (table.unpack or unpack)(idx)
end

function MaxIdx(...)
	-- build table from arguments
	tbl = {}
	for i = 1, #arg do
		table.insert(tbl, arg[i])
	end
	-- sort indices
	-- select 4 max indices
	mx1idx, mx2idx, mx3idx, mx4idx = indexsort(tbl)
	mx = mx1idx
	if mx1idx == last_mx1idx then
		if mx2idx == last_mx2idx then
			if mx3idx == last_mx3idx then
				mx = mx4idx
			else
				mx = mx3idx
			end
		else
			mx = mx2idx
		end
	end
	last_mx1idx = mx1idx
	last_mx2idx = mx2idx
	last_mx3idx = mx3idx
	last_mx4idx = mx4idx
	return mx
end