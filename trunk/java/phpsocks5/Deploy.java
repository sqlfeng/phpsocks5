package phpsocks5;

import java.io.*;
import java.util.*;

public class Deploy
{

	public static void main(String[] args) throws NumberFormatException, IOException
	{
		{
			FileOutputStream ftmp = new FileOutputStream("deploy.tmp");
			ftmp.close();
		}
		BufferedReader brStdIn = new BufferedReader(new InputStreamReader(System.in));
		System.out.print("Please select language.  1 - English, 2 - Chinese: ");
		Properties prompt = new Properties();
		switch(Integer.parseInt(brStdIn.readLine()))
		{
		case 1:
			prompt.load(Deploy.class.getResourceAsStream("deploy_en.properties"));
			break;
		case 2:
			prompt.load(Deploy.class.getResourceAsStream("deploy_zh.properties"));
			break;
		default:
			System.err.println("Input wrong in select language.");
			return;
		}
		Map<String, String> mconfig = new LinkedHashMap<String, String>();
		System.out.print(prompt.getProperty("inputdbhost"));
		mconfig.put("dbhost", brStdIn.readLine());
		mconfig.put("dbport", "3306");
		System.out.print(prompt.getProperty("inputdbuser"));
		mconfig.put("dbuser", brStdIn.readLine());
		System.out.print(prompt.getProperty("inputdbpass"));
		mconfig.put("dbpass", brStdIn.readLine());
		System.out.print(prompt.getProperty("inputdbname"));
		mconfig.put("dbname", brStdIn.readLine());
		char[] keySeed = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz".toCharArray();
		StringBuilder sb = new StringBuilder();
		for(int i = 0; i < 5000; i++)
			sb.append(keySeed[(int) (Math.random() * keySeed.length)]);
		mconfig.put("secretkey", sb.toString());
		mconfig.put("debuginfo", "True");
		sb = new StringBuilder();
		for(int i = 0; i < 30; i++)
			sb.append(keySeed[(int) (Math.random() * keySeed.length)]);
		mconfig.put("prefix", sb.toString());
		sb = new StringBuilder();
		for(int i = 0; i < 30; i++)
			sb.append(keySeed[(int) (Math.random() * keySeed.length)]);
		mconfig.put("postfix", sb.toString());
		mconfig.put("dbprefix", "phpsocks5_");
		mconfig.put("invstep", "100000");
		mconfig.put("invmax", "3000000");
		mconfig.put("localport", "10080");
		mconfig.put("localhost", "127.0.0.1");
		mconfig.put("polipoport", "10088");
		mconfig.put("polipohost", "127.0.0.1");
		List<Map.Entry<String, String>> lconfig = new ArrayList<Map.Entry<String, String>>();
		lconfig.addAll(mconfig.entrySet());
		while(true)
		{
			System.out.println(prompt.getProperty("allconfig"));
			int i;
			for(i = 0; i < lconfig.size(); i++)
			{
				Map.Entry<String, String> e = lconfig.get(i);
				System.out.print(i + 1);
				System.out.print(" - ");
				System.out.print(e.getKey());
				System.out.print(": ");
				System.out.println(e.getValue());
			}
			System.out.println(prompt.getProperty("writeconfig"));
			System.out.printf(prompt.getProperty("selectoption"), i);
			String seledopt = brStdIn.readLine();
			if(seledopt.equalsIgnoreCase("w"))
				break;
			int seleditm = Integer.parseInt(seledopt) - 1;
			Map.Entry<String, String> e = lconfig.get(seleditm);
			System.out.printf(prompt.getProperty("editconfig"), e.getKey(), e.getValue());
			e.setValue(brStdIn.readLine());
		}
		{
			File fbak = new File("../php/socks5.php.bak");
			fbak.delete();
			File fcur = new File("../php/socks5.php");
			fcur.renameTo(fbak);
		}
		{
			BufferedReader fbak = new BufferedReader(new InputStreamReader(new FileInputStream("../php/socks5.php.bak")));
			PrintStream fcur = new PrintStream(new FileOutputStream("../php/socks5.php"));
			String[] ssrvCfg = new String[]{"dbhost", "dbport", "dbuser", "dbpass", "dbname", "secretkey", "prefix", "postfix", "dbprefix"};
			String[] nsrvCfg = new String[]{"debuginfo", "invstep", "invmax"};
			while(true)
			{
				String line = fbak.readLine();
				if(line == null)
					break;
				if(line.equals("?>"))
				{
					fcur.print(line);
					break;
				}
				int i;
				for(i = 0; i < ssrvCfg.length; i++)
					if(line.startsWith("$" + ssrvCfg[i] + " "))
						fcur.println("$" + ssrvCfg[i] + " = '" + mconfig.get(ssrvCfg[i]) + "';");
				if(i == ssrvCfg.length)
					continue;
				for(i = 0; i < nsrvCfg.length; i++)
					if(line.startsWith("$" + ssrvCfg[i] + " "))
						fcur.println("$" + ssrvCfg[i] + " = " + mconfig.get(ssrvCfg[i]) + ";");
				if(i == nsrvCfg.length)
					continue;
				fcur.println(line);
			}
			fcur.flush();
			fcur.close();
			System.out.print(prompt.getProperty("uploadphp"));
		}
		{
			System.out.print(prompt.getProperty("inputserverurl"));
			mconfig.put("serverurl", brStdIn.readLine());
			File fbak = new File("phpsocks5.properties.bak");
			fbak.delete();
			File fcur = new File("phpsocks5.properties");
			fcur.renameTo(fbak);
		}
		{
			PrintStream fcur = new PrintStream(new FileOutputStream("phpsocks5.properties"));
			String[] cCfg = new String[]{"serverurl", "secretkey", "prefix", "postfix", "localport", "localhost"};
			for(int i = 0; i < cCfg.length; i++)
				fcur.println(cCfg[i] + " = " + mconfig.get(cCfg[i]));
			fcur.flush();
			fcur.close();
		}
		{
			File fbak = new File("polipo.conf.bak");
			fbak.delete();
			File fcur = new File("polipo.conf");
			fcur.renameTo(fbak);
		}
		{
			PrintStream fcur = new PrintStream(new FileOutputStream("polipo.conf"));
			fcur.println("socksParentProxy=127.0.0.1:" + mconfig.get("localport"));
			fcur.println("proxyHost=" + mconfig.get("polipohost"));
			fcur.println("proxyPort=" + mconfig.get("polipoport"));
			fcur.flush();
			fcur.close();
		}
		System.out.print(prompt.getProperty("runtips"));
	}

}
