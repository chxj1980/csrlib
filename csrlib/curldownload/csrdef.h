#ifndef _CSR_DEF_H_
#define _CSR_DEF_H_

//��Ա������������
#define TPGETSET(ObjName, ObjValue)				 \
	ObjName ObjValue;								  \
	public:												\
	inline ObjName Get##ObjValue(){ return ObjValue; }   \
	inline void Set##ObjValue(ObjName obj){ ObjValue = obj; }		\
	private:

#endif // _CSR_DEF_H_
