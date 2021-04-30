#pragma once
class Object;

class ObjectStructure abstract 
{
public:
	virtual void AttachObject(Object* pObject);
	virtual void AttachObjectTo(Object* pObject);
	virtual void DetachObject(Object* pObject);
	virtual void DetachObjectFrom();

	Object* GetParent() const;
	void SetParent(Object* pObject);

protected:
	ObjectStructure(Object* pObject);
	virtual ~ObjectStructure();

	ObjectStructure(ObjectStructure& other) = delete;
	ObjectStructure(ObjectStructure&& other) = delete;
	ObjectStructure operator=(ObjectStructure& other) = delete;
	ObjectStructure& operator=(ObjectStructure&& other) = delete;

	Object* m_pParent;
	Object* m_pChildInstance;
	std::vector<Object*> m_Objects;
};