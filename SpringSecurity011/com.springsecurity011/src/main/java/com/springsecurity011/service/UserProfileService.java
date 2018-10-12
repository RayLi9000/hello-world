package com.springsecurity011.service;

import java.util.List;

import com.springsecurity011.model.UserProfile;

public interface UserProfileService 
{
	public UserProfile findById(int id);
	public UserProfile findByType(String type);
	public List<UserProfile> findAll();
}
