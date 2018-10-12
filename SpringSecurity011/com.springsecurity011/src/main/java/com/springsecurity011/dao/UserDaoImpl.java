package com.springsecurity011.dao;

import java.util.List;

import org.hibernate.Hibernate;
import org.hibernate.Query;
import org.hibernate.Session;
import org.hibernate.SessionFactory;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Repository;

import com.springsecurity011.model.User;

@Repository("userDao")
public class UserDaoImpl implements UserDao 
{
	static final Logger logger = LoggerFactory.getLogger(UserDaoImpl.class);
	
	@Autowired
	private SessionFactory sessionFactory;
	
	private Session getSession()
	{
		return sessionFactory.getCurrentSession();
	}
	
	/**
	 * Hibernate.initialize()适用于解决延时加载，因为在Model中， User的UserPrfile是延时加载， 延时加载不会马上发出sql语句去查询数据库，
	 * 只有当用到的时候，Hibernate才会发出SQL语句去数据库中取数据，达到减少系统性能的消耗。因为延时加载是先通过CGLIB生成一个代理对象，
	 * 万一Session关闭了的话， 代理不想就会拿不到数据，从而报一个NoSessionException, 所以Hibernate.initialize()相当于预查询了一下，
	 * 这样子的话尽管Session关闭，代理对象还能到数据库中查找数据
	 * 在程序里面可以用强制加载的方法Hibernate.initialize(Object proxy) 方法强制加载这样就相当于动态改变为lazy=false
	 * 但在使用时需要注意的一点是：其中的proxy是持久对象的关联对象属性，比如A实体，你要把A的关联实体B也检出，则要写Hibernate.initialize(a.b)。
	 * Hibernate.initialize相当于OpenSessionInView
	 */
	@Override
	public User findById(int id) 
	{
		User user = getSession().get(User.class, id);
		if (user != null)
			Hibernate.initialize(user.getUserProfiles());
		return user; 
	}

	@Override
	public User findBySSO(String sso) 
	{
		logger.info("SSO : {}", sso);
		String hql = "from User user where user.ssoId = :ssoId";
		Query q = getSession().createQuery(hql).setParameter("ssoId", sso);
		User user =  (User) q.uniqueResult();
		if (user != null)
			Hibernate.initialize(user.getUserProfiles());
		return user;
	}

	@Override
	public void save(User user) 
	{
		getSession().save(user); //因为User和UserProfile多对多关系，而且由一张表维护这个关系，所以当save一个User的时候，也会向维护表插入一个数据，因为有外键关联
	}

	@Override
	public void deleteBySSO(String sso) 
	{
		String hql = "delete from User user where user.ssoId = :ssoId";
		Query q = getSession().createQuery(hql).setParameter("ssoId", sso);
		q.executeUpdate();
	}
	
	/**
	 * 因为在lIstAllUsers的时候不需要查看User的Profile，所以采用Fectch lazy可以减少系统性能的消耗，
	 * 但是需要查看用户proifile的时候就需要用Hibernate.initialize()来动态改变fetch lazy = false
	 */
	@Override
	public List<User> findAllUsers() 
	{
		String hql = "from User";
		Query q = getSession().createQuery(hql);
		List<User> users = (List<User> )q.list();
		return users;
	}

}
