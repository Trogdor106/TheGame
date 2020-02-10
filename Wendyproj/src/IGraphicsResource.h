/*
	File: IGraphicsResource.h
	Date: Jan 10, 2020
	Author: Shawn Matthews
	Description:
	   Provides a base class for graphics resources which includes a debug name
	   and a renderer handle
*/
#pragma once
#include <string>
#include <cstdint>
#include <memory>

#define NoCopy(TypeName) \
TypeName(const TypeName& other) = delete; \
TypeName& operator =(const TypeName& other) = delete;

#define NoMove(TypeName) \
TypeName(const TypeName&& other) = delete; \
TypeName& operator =(const TypeName&& other) = delete;

#define GraphicsClass(TypeName)\
	typedef std::shared_ptr<TypeName> Sptr;\
	NoMove(TypeName);\
	NoCopy(TypeName); 


/*
	Represents the base class for all of our graphics resources
*/
class IGraphicsResource {
public:
	GraphicsClass(IGraphicsResource);

	virtual ~IGraphicsResource() = default;

	/*
		Gets the debugging name associated with this graphics resource
		@returns The debug name of the resource
	*/
	const std::string& GetDebugName() const { return myDebugName; }
	/*
		Sets the debugging name associated with this graphics resource
		@param value The new name for the resource
	*/
	virtual void SetDebugName(const std::string& value) { myDebugName = value; }

	/*
		Gets the renderer ID of this resource. This is the ID that the underlying
		renderer will assign to the resource upon creation (for instance, this will
		be a handle that is populated by a glCreate* command)
		@returns A 32 bit handle for this resource
	*/
	uint32_t GetRenderID() const { return myRendererID; }

protected:
	IGraphicsResource() = default;

	std::string myDebugName;
	uint32_t    myRendererID;
};
