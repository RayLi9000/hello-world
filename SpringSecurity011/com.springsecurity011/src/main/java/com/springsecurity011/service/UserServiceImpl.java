package com.springsecurity011.service;

import java.util.List;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.security.crypto.password.PasswordEncoder;
import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Transactional;

import com.springsecurity011.dao.UserDao;
import com.springsecurity011.model.User;

@Service("userService")
@Transactional
public class UserServiceImpl implements UserService
{
	@Autowired
	private UserDao userDao;
	
	@Autowired
	private PasswordEncoder passwordEncoder; //注入一个加密器，用于加密用户的密码
	
	@Override
	public User findById(int id) 
	{
		return userDao.findById(id);
	}

	@Override
	public User findBySSO(String sso)
	{
		return userDao.findBySSO(sso);
	}

	@Override
	public void saveUser(User user) 
	{
		//先加密用户的密码
		user.setPassword(passwordEncoder.encode(user.getPassword()));
		userDao.save(user);
	}
	
	/**
	 * 因为在整个类上面标注有@Transactional注解，所以所有的方法都有一个事务管理
	 * 所以没必要明显地调用Hibernate的update， 只要在事务提交前，更新后的对象就会跟Hibernate中Session缓存
	 * 中的对象作比较，要是不一致的话，就会发出update语句，更新数据库，最后提交事务
	 */
	@Override
	public void updateUser(User user)
	{
		User entity = userDao.findById(user.getId());
		if (entity != null)
		{
			entity.setSsoId(user.getSsoId());
			if (!user.getPassword().equals(entity.getPassword()))
				entity.setPassword(passwordEncoder.encode(user.getPassword()));
			entity.setFirstName(user.getFirstName());
			entity.setLastName(user.getLastName());
			entity.setEmail(user.getEmail());
			entity.setUserProfiles(user.getUserProfiles());
		}
	}

	@Override
	public void deleteUserBySSO(String sso) 
	{
		userDao.deleteBySSO(sso);
	}

	@Override
	public List<User> findAllUsers()
	{
		return userDao.findAllUsers();
	}

	@Override
	public boolean isUserSSOUnique(Integer id, String sso) 
	{
		User user = userDao.findBySSO(sso);
		return (user == null || (id != null && (user.getId() == id)));
	}

}
