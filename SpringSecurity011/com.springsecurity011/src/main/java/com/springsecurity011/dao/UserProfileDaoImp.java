package com.springsecurity011.dao;

import java.util.List;

import org.hibernate.Query;
import org.hibernate.Session;
import org.hibernate.SessionFactory;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Repository;

import com.springsecurity011.model.UserProfile;

@Repository("userProfileDao")
public class UserProfileDaoImp implements UserProfileDao 
{
	@Autowired
	private SessionFactory sessionFactory;
	
	private Session getSession()
	{
		return sessionFactory.getCurrentSession();
	}
	
	@Override
	public List<UserProfile> findAll() 
	{
		String hql = "from UserProfile";
		Query q = getSession().createQuery(hql);
		return (List<UserProfile>)q.list();
	}

	@Override
	public UserProfile findByType(String type) 
	{
		String hql = "from UserProfile userProfile where userProfile.type = :type";
		Query q = getSession().createQuery(hql).setParameter("type", type);
		return (UserProfile) q.uniqueResult();
	}

	@Override
	public UserProfile findById(int id)
	{
		return getSession().get(UserProfile.class, id);
	}

}
