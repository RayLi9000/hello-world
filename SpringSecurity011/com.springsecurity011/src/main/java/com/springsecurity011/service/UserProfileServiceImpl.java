package com.springsecurity011.service;

import java.util.List;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Transactional;

import com.springsecurity011.dao.UserProfileDao;
import com.springsecurity011.model.UserProfile;

@Service("userProfileService")
@Transactional
public class UserProfileServiceImpl implements UserProfileService 
{
	@Autowired
	private UserProfileDao userProfileDao;
	
	@Override
	public UserProfile findById(int id) 
	{
		return userProfileDao.findById(id);
	}

	@Override
	public UserProfile findByType(String type)
	{
		return userProfileDao.findByType(type);
	}

	@Override
	public List<UserProfile> findAll()
	{
		return userProfileDao.findAll();
	}

}
