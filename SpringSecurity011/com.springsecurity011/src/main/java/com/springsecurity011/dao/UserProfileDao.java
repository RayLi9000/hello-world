package com.springsecurity011.dao;

import java.util.List;

import com.springsecurity011.model.UserProfile;

public interface UserProfileDao 
{
	public List<UserProfile> findAll();
	public UserProfile findByType(String type);
	public UserProfile findById(int id);
	
}
