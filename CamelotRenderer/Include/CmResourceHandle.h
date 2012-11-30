#pragma once

#include "CmIReflectable.h"

namespace CamelotEngine
{
	template <typename T>
	class ResourceHandle;

	struct CM_EXPORT ResourceHandleData : public IReflectable
	{
		ResourceHandleData()
			:mIsResolved(false)
		{ }

		std::shared_ptr<Resource> mPtr;
		String mUUID;
		bool mIsResolved;

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/
	public:
		friend class ResourceHandleDataRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;		
	};

	class CM_EXPORT ResourceHandleBase : public IReflectable
	{
	public:
		/**
		 * @brief	Checks if the resource is loaded
		 */
		bool isResolved() const { return mData->mIsResolved; }

	protected:
		ResourceHandleBase();

		std::shared_ptr<ResourceHandleData> mData;

		void init(Resource* ptr);
		void init(std::shared_ptr<Resource> ptr);

		template <typename T1>
		void init(const ResourceHandle<T1>& ptr)
		{
			mData = ptr.mData;
		}
	private:
		friend class Resources;
		/**
		 * @brief	Sets the resolved flag to true. Should only be called
		 * 			by Resources class after loading of the resource is fully done.
		 */
		void resolve(std::shared_ptr<Resource> ptr);

		/**
		 * @brief	Sets an uuid of the ResourceHandle. Should only be called by
		 * 			Resources class.
		 */
		void ResourceHandleBase::setUUID(const String& uuid);

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/
	public:
		friend class ResourceHandleRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;
	};

	template <typename T>
	class ResourceHandle : public ResourceHandleBase
	{
	public:
		ResourceHandle()
			:ResourceHandleBase()
		{	}

		explicit ResourceHandle(T* ptr)
			:ResourceHandleBase()
		{
			init(ptr);
		}

		ResourceHandle(std::shared_ptr<T> ptr)
			:ResourceHandleBase()
		{
			init(ptr);
		}

		template <typename T1>
		ResourceHandle(const ResourceHandle<T1>& ptr)
			:ResourceHandleBase()
		{
			init(ptr);
		}

		operator ResourceHandle<Resource>() 
		{
			return ResourceHandle<Resource>(mData->mPtr); 
		}

		// TODO Low priority - User can currently try to access these even if resource ptr is not resolved
		T* get() const 
		{ 
			if(!isResolved()) 
				return nullptr; 
			
			return static_cast<T*>(mData->mPtr.get()); 
		}
		T* operator->() const { return get(); }
		T& operator*() const { return *get(); }

		std::shared_ptr<T> getInternalPtr() { if(!isResolved()) return nullptr; return std::static_pointer_cast<T>(mData->mPtr); }

		template<class _Ty>
		struct CM_Bool_struct
		{
			int _Member;
		};

		// Conversion to bool
		// (Why not just directly convert to bool? Because then we can assign pointer to bool and that's weird)
		operator int CM_Bool_struct<T>::*() const
		{
			return ((isResolved() && (mData->mPtr.get() != 0)) ? &CM_Bool_struct<T>::_Member : 0);
		}
	};

	template<class _Ty1, class _Ty2>
		ResourceHandle<_Ty1> static_resource_cast(const ResourceHandle<_Ty2>& other)
	{	
		return ResourceHandle<_Ty1>(other);
	}

	template<class _Ty1, class _Ty2>
	bool operator==(const ResourceHandle<_Ty1>& _Left, const ResourceHandle<_Ty2>& _Right)
	{	
		return (_Left.get() == _Right.get());
	}

	template<class _Ty1, class _Ty2>
	bool operator!=(const ResourceHandle<_Ty1>& _Left, const ResourceHandle<_Ty2>& _Right)
	{	
		return (!(_Left == _Right));
	}
}