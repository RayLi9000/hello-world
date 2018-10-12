package com.springsecurity011.model;

import java.io.Serializable;
import java.util.Date;

import javax.persistence.Column;
import javax.persistence.Entity;
import javax.persistence.Id;
import javax.persistence.Table;
import javax.persistence.Temporal;
import javax.persistence.TemporalType;

/**
 * 这个类用于Hibernate映射到数据库当使用RememberMe功能时
 */

@Entity
@Table(name = "PERSISTENT_LOGINS")
public class PersistentLogin implements Serializable
{

	@Id
	public String series;
	
	@Column(name = "USERNAME", unique = true, nullable = false)
	private String username;
	
	@Column(name = "TOKEN", unique = true, nullable = false)
	private String token;
	
	/**
	 * @Temporal用于自动时间格式转换
	 * 第一种：@Temporal(TemporalType.DATE)——》实体类会封装成日期“yyyy-MM-dd”的 Date类型。
	          第二种：@Temporal(TemporalType.TIME)——》实体类会封装成时间“hh-MM-ss”的 Date类型。
	          第三种：@Temporal(TemporalType.TIMESTAMP)——》实体类会封装成完整的时间“yyyy-MM-dd hh:MM:ss”的 Date类型。
	 */
	@Temporal(TemporalType.TIMESTAMP)
	private Date last_used;

	public String getSeries() 
	{
		return series;
	}

	public void setSeries(String series) 
	{
		this.series = series;
	}

	public String getUsername()
	{
		return username;
	}

	public void setUsername(String username)
	{
		this.username = username;
	}

	public String getToken() {
		return token;
	}

	public void setToken(String token)
	{
		this.token = token;
	}

	public Date getLast_used() 
	{
		return last_used;
	}

	public void setLast_used(Date last_used) 
	{
		this.last_used = last_used;
	}
	
}
