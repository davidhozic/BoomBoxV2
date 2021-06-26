


#define readBIT(reg, bit)				(		(	(reg >> bit) & 0x1	)												)
#define writeBIT(reg, bit, val)			(		(reg = val ? ( reg | (0x1 << bit) ) : ( reg  &  ~(0x1 << bit)))			)