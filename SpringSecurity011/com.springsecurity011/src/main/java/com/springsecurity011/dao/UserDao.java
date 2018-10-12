package com.springsecurity011.dao;

import java.util.List;

import com.springsecurity011.model.User;

public interface UserDao 
{
	public User findById(int id);
	public User findBySSO(String sso);
	public void save(User user);
	public void deleteBySSO(String sso);
	public List<User> findAllUsers();
}
