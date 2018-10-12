package com.springsecurity011.dao;

import java.util.Date;

import org.hibernate.Query;
import org.hibernate.Session;
import org.hibernate.SessionFactory;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.security.web.authentication.rememberme.PersistentRememberMeToken;
import org.springframework.security.web.authentication.rememberme.PersistentTokenRepository;
import org.springframework.stereotype.Repository;
import org.springframework.transaction.annotation.Transactional;

import com.springsecurity011.model.PersistentLogin;


/**
 * 这个类是自定的PersistentTokenRepository 实现 PersistentTokenRepository接口
 * 用于RememberMe 功能的实现
 * 是通过Hibernate映射自己数据库，实现Token的增删改查
 *
 */
@Repository("tokenRepository")
@Transactional
public class HibernateTokenRepositoryImpl implements PersistentTokenRepository
{
	
	static final Logger logger = LoggerFactory.getLogger(HibernateTokenRepositoryImpl.class);
	
	@Autowired
	private SessionFactory sessionFactory;
	
	private Session getSession()
	{
		return sessionFactory.getCurrentSession();
	}
	
	@Override
	public void createNewToken(PersistentRememberMeToken token) 
	{
		logger.info("Creating Token for User : {}", token.getUsername());
		PersistentLogin persistentLogin = new PersistentLogin();
		persistentLogin.setUsername(token.getUsername());
		persistentLogin.setSeries(token.getSeries());
		persistentLogin.setToken(token.getTokenValue());
		persistentLogin.setLast_used(token.getDate());
		getSession().save(persistentLogin);
	}

	@Override
	public void updateToken(String seriesId, String tokenValue, Date lastUsed) 
	{
		logger.info("Updating Token for seriedId : {}", seriesId);	
		String hql = "from PersistentLogin persistentLogin where persistentLogin.series = :seriesId";
		Query q = getSession().createQuery(hql).setParameter("seriedId", seriesId);
		PersistentLogin persistentLogin = (PersistentLogin) q.uniqueResult();
		persistentLogin.setToken(tokenValue);
		persistentLogin.setLast_used(lastUsed);
		getSession().update(persistentLogin);
	}

	@Override
	public PersistentRememberMeToken getTokenForSeries(String seriesId) {
		logger.info("Fetch Token if any for seriesId : {}", seriesId);
		String hql = "from PersistentLogin persistentLogin where persistentLogin.series = :seriesId";
		Query q = getSession().createQuery(hql).setParameter("seriesId", seriesId);
		PersistentLogin persistentLogin =  (PersistentLogin) q.uniqueResult();
		if (persistentLogin == null) 
		{
			logger.info("Token not found");
			return null;
		}
		else
			return new PersistentRememberMeToken(persistentLogin.getUsername(), persistentLogin.getSeries(),
							persistentLogin.getToken(), persistentLogin.getLast_used());
	}

	@Override
	public void removeUserTokens(String username) 
	{
		logger.info("Removing Token if any for user : {}", username);
		String hql = "from PersistentLogin persistentLogin where persistentLogin.username = :username";
		Query q = getSession().createQuery(hql).setParameter("username", username);
		PersistentLogin persistentLogin = (PersistentLogin) q.uniqueResult();
		if (persistentLogin != null)
		{
			logger.info("remember was selected");
			getSession().delete(persistentLogin);
		}
	}

}
