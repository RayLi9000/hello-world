package com.springsecurity011.service;

import java.util.List;

import com.springsecurity011.model.User;

public interface UserService 
{
	public User findById(int id);
	public User findBySSO(String sso);
	public void saveUser(User user);
	public void updateUser(User user);
	public void deleteUserBySSO(String sso);
	public List<User> findAllUsers();
	public boolean isUserSSOUnique(Integer id, String sso);
	
}
